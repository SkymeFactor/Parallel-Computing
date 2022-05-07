#include <algorithm>
#include "ezocl_core.h"



namespace ezocl {


std::vector<cl_platform_id> DeviceManager::platforms = std::vector<cl_platform_id>();
std::vector<Device> DeviceManager::devices = std::vector<Device>();


void DeviceManager::check_errors(const cl_int retrun_status, const std::string_view msg) {
    if (retrun_status != CL_SUCCESS) {
        throw Error(std::string(msg).c_str());
    }
};


void DeviceManager::list_platforms() {
    cl_uint num_platforms;
    cl_int ret;

    ret = clGetPlatformIDs(0, NULL, &num_platforms);
    check_errors(ret, "Unable to get number of opencl platforms");

    platforms.resize((size_t)num_platforms);

    ret = clGetPlatformIDs(num_platforms, platforms.data(), NULL);
    check_errors(ret, "Unable to get opencl platforms");
};


void DeviceManager::list_devices() {
    cl_device_id* platform_devices;
    cl_int ret;

    for (auto p: platforms) {
        cl_uint num_devices;
        ret = clGetDeviceIDs(p, CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
        check_errors(ret, "Unable to get number of opencl devices");
        platform_devices = new cl_device_id[num_devices];
        ret |= clGetDeviceIDs(p, CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_GPU, num_devices, platform_devices, NULL);
        check_errors(ret, "Unable to get opencl devices");

        for (int i = 0; i < num_devices; ++i)
            devices.emplace_back(Device(platform_devices[i], p));
        
        delete[] platform_devices;
    }
    std::sort(devices.begin(), devices.end());
};


std::vector<Device> DeviceManager::getDevices() {
    if (devices.size() == 0) {
        if (platforms.size() == 0)
            list_platforms();
        
        list_devices();
    }
    
    return devices;
};


}   // end of namespace ezocl