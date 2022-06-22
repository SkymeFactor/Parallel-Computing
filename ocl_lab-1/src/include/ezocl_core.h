#pragma once

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define LOCAL_SIZE 16

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string_view>
#include <vector>
#include <memory>
#include <type_traits>
#include <cmath>
#include <CL/opencl.h>



namespace ezocl {


typedef enum {
    IN_BUFFER = CL_MEM_READ_ONLY,
    IN_OUT_BUFFER = CL_MEM_READ_WRITE,
    OUT_BUFFER = CL_MEM_WRITE_ONLY,
    IN_SCALAR
} BufferType;

typedef enum {
    TYPE_dGPU,
    TYPE_iGPU,
    TYPE_CPU,
    UNKNOWN
} DeviceType;

// General purpose error class
class Error;
// OpenCL Device wrapper
class Device;
// OpenCL Devices manager, automatizes devices retrieval
class DeviceManager;
// OpenCL Kernel wrapper, accepts buffers
class Kernel;
// OpenCL Buffer wrapper, every kernel argument must be wrapped into a buffer
class Buffer;
// OpenCL Program wrapper (contains context and command_queue related features)
class Program;


// Type traits stuff to make sure that we have the same class in types T and U
template <typename T, typename U>
struct decay_equal :
    std::is_same<typename std::decay<T>::type, U>::type
{};


template <class... Args>
std::vector<std::size_t> makeGlobalNDRange(std::vector<std::size_t> local_worksize, Args&&... args) {
    std::vector<std::size_t> NDRange(local_worksize.size());
    unsigned iter = 0;
    
    ([&] (const auto& dimension) {
        const std::size_t local_size = std::ceil(local_worksize[iter]);
        const std::size_t dim = std::ceil(dimension);
        NDRange[iter] = dim % local_size == 0 ? dim : dim + local_size - dim % local_size;
        ++iter;
    } (std::forward<Args>(args)), ...);

    return NDRange;
};


class Error : public std::exception {
    std::string what_msg;
public:
    Error(const char msg[]) : what_msg(msg) {};
    const char* what() const noexcept override { return what_msg.c_str(); };
};


class Device {
private:
    cl_device_id handle {0};
    cl_platform_id platform_handle {CL_NONE};
    DeviceType device_type {DeviceType::UNKNOWN};
    cl_device_type type {CL_NONE};
    std::string name {""};
    std::string vendor {""};
public:
    Device(cl_device_id& device, cl_platform_id& platform): platform_handle{platform} {
        this->handle = device;
        init_properties();
    };
    Device() = default;
    ~Device() { releaseHandle(); };

    void init_properties();
    bool operator<(const Device &rhs) const;
    
    cl_device_id& acquireHandle() { return handle; };
    void releaseHandle() { if (handle != 0) { clReleaseDevice(handle); handle = 0; } };
    
    cl_device_id get_device_handle() const { return handle; };
    cl_platform_id get_platform_handle() const { return platform_handle; };
    DeviceType get_device_type() const { return device_type; };
    cl_device_type get_cl_device_type() const { return type; };
    std::string get_device_name() const { return name; };
    std::string_view get_device_vendor() const { return vendor; };
};


class DeviceManager {
private:
    static std::vector<cl_platform_id> platforms;
    static std::vector<Device> devices;

    static void check_errors(const cl_int retrun_status, const std::string_view msg);
    static void list_platforms();
    static void list_devices();
public:
    static std::vector<Device> getDevices();
};


class Buffer {
private:
    void* pointer {nullptr};
    cl_mem handle {0};
    cl_event time_profiler[2] {0};
    std::size_t size {0};
    std::size_t mem_type {0};
public:
    Buffer(void* pointer, std::size_t size, BufferType mem_type):
            pointer{pointer}, size{size}, mem_type{static_cast<std::size_t>(mem_type)}
    {};
    
    void acquireHandle(cl_context& ctx, cl_command_queue& cq);
    void realeaseHandle(cl_command_queue& cq);

    std::pair<std::size_t, std::size_t> getTransferToDeviceTime();
    std::pair<std::size_t, std::size_t> getTransferFromDeviceTime();

    void* getPointer() { if (mem_type == BufferType::IN_SCALAR) return pointer; else return (void*)&handle; };
    std::size_t getSize() { if (mem_type == BufferType::IN_SCALAR) return size; else return sizeof(cl_mem); };
};


class Kernel {
    cl_kernel handle {0};
    cl_event time_profiler {0};
    std::size_t execution_time {0};
    std::string kernel_name;
    std::vector<std::size_t> global_work_size;
    std::vector<std::size_t> local_work_size;
    std::vector<std::shared_ptr<Buffer>> arguments;
public:
    template <typename T = std::vector<std::size_t>, class... Args/*,
        std::enable_if_t<(decay_equal<Args, Buffer>::value && ...) , bool> = true*/>
    Kernel(std::string_view kernel_name, T global_work_size, T local_work_size, Args... args):
            kernel_name{kernel_name},
            global_work_size{std::forward<T>(global_work_size)},
            local_work_size{std::forward<T>(local_work_size)},
            arguments{std::make_shared<Buffer>(std::forward<Buffer>(args))...}
    {}
    Kernel() = delete;
    ~Kernel() { releaseHandle(); };

    void enqueueArgs(cl_context& ctx, cl_command_queue& cq);
    void releaseArgs(cl_command_queue& cq);
    void setArgsToKernel();
    
    void acquireHandle(cl_program& program_handle);
    void releaseHandle();

    void runKernel(cl_command_queue& cq);

    std::size_t getExecutionTime();
    std::size_t getTotalTime();
};


class Program {
private:
    std::string program_source;

    Device device;
    std::vector<Kernel> kernels;
    cl_context context {0};
    cl_program program {0};
    cl_command_queue command_queue {0};

    std::vector<std::size_t> total_execution_time;
    std::vector<std::size_t> kernel_execution_time;

    void teardown();
public:
    template <typename DeviceType, class... KernelTypes/*,
                std::enable_if_t<decay_equal<DeviceType, ezocl::Device>::value &&
                (decay_equal<KernelTypes, ezocl::Kernel>::value && ...) , bool> = true*/>
    explicit Program(std::string filename, DeviceType device, KernelTypes... kernels) :
                device{std::forward<DeviceType>(device)},
                kernels{std::forward<KernelTypes>(kernels)...}
    {
        std::ifstream fin(filename);
        if (!fin.is_open()) throw std::fstream::failure("Unable to read source code from file " + filename);
        std::stringstream ss;
        ss << fin.rdbuf();
        program_source = ss.str();
    }
    Program(const Program&) = delete;
    Program(Program&&) = delete;
    Program& operator=(const Program&) = delete;
    
    ~Program() { teardown(); };

    void execute(std::string build_options = "");
    std::vector<std::size_t> getKernelExecutionTime() {return kernel_execution_time; };
    std::vector<std::size_t> getTotalKernelTime() {return total_execution_time; };
};

}   // end of namespace ezocl