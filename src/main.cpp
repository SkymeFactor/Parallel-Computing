#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <cstdlib>
#include <iomanip>

#include "ezocl_core.h"
#include "utilities.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif



int main(int argc, char* argv[]) {
    if (argc < 5)
        return EXIT_FAILURE;
    
    // Initialize
    int ocl_device_number;
    int algorithm_implementation;
    
    try {
        ocl_device_number = std::stoi(argv[1]);
        algorithm_implementation = std::stoi(argv[4]);
    } catch (...) {
        std::cerr << "[ ERROR ]: Integral parameter expected, got NaN\n";
        return EXIT_FAILURE;
    }
    
    std::string in_filename {argv[2]};
    std::string out_filename {argv[3]};
    std::string ocl_build_options {"-cl-std=CL1.2 -DBLOCK_SIZE=" + std::to_string(LOCAL_SIZE)};
    std::string kernel_filename {"src/kernels/floatMatMul.cl"};
    std::string kernel_name {""};
    
    std::vector<std::size_t> local_worksize {LOCAL_SIZE, LOCAL_SIZE};
    unsigned vector_size = 1;

    try {
        switch (algorithm_implementation) {
            case 1: kernel_name = "matMulSimple"; break;
            case 2: kernel_name = "matMulBlocked"; break;
            case 3:
                kernel_name = "matMulBlockedVectorized";
                vector_size = 8;
                local_worksize = {LOCAL_SIZE, LOCAL_SIZE / vector_size};
            break;
            default: throw std::invalid_argument("Invalid algorithm version");
        }
    
        // Read matrices from file
        auto [mat_in1, mat_in2] = parse_matrices_file(in_filename); //generate_test_data(1000, 1000, 1000);
        
        auto resulting_width = mat_in2.getWidth(),
            resulting_height = mat_in1.getHeight();
        
        mat_in1 = make_zero_padding(
            mat_in1,
            LOCAL_SIZE - mat_in1.getHeight() % LOCAL_SIZE,
            LOCAL_SIZE - mat_in1.getWidth() % LOCAL_SIZE
        );
        mat_in2 = make_zero_padding(
            mat_in2,
            LOCAL_SIZE - mat_in2.getHeight() % LOCAL_SIZE,
            LOCAL_SIZE - mat_in2.getWidth() % LOCAL_SIZE
        );
        
        decltype(mat_in1) mat_out;
        mat_out.setSize(mat_in1.getHeight(), mat_in2.getWidth());

#ifdef DEBUG
        Matrix<float> mat_eval = mat_mul_cpu(mat_in1, mat_in2);
#endif

        // OpenCL set-up
        auto ocl_devices = ezocl::DeviceManager::getDevices();

        if (ocl_devices.size() == 0) throw std::runtime_error("No devices found");
        if (ocl_device_number > ocl_devices.size() - 1) ocl_device_number = 0;
        
        cl_uint hA = mat_in1.getHeight();
        cl_uint wA = mat_in1.getWidth();
        cl_uint wB = mat_in2.getWidth();
        ezocl::Buffer heightA{&hA, sizeof(hA), ezocl::BufferType::IN_SCALAR};
        ezocl::Buffer widthA {&wA, sizeof(wA), ezocl::BufferType::IN_SCALAR};
        ezocl::Buffer widthB {&wB, sizeof(wB), ezocl::BufferType::IN_SCALAR};
        ezocl::Buffer matA {&mat_in1[0][0], mat_in1.getSizeInBytes(), ezocl::BufferType::IN_BUFFER};
        ezocl::Buffer matB {&mat_in2[0][0], mat_in2.getSizeInBytes(), ezocl::BufferType::IN_BUFFER};
        ezocl::Buffer matC {&mat_out[0][0], mat_out.getSizeInBytes(), ezocl::BufferType::OUT_BUFFER};

        ezocl::Kernel kernel {
            kernel_name,
            ezocl::makeGlobalNDRange(local_worksize, mat_in2.getWidth(), mat_in1.getHeight() / static_cast<double>(vector_size)),
            local_worksize,
            heightA, widthA, widthB,
            matA, matB, matC
        };

        ezocl::Program my_program(kernel_filename, ocl_devices[ocl_device_number], kernel);
        my_program.execute(ocl_build_options);
        
#ifdef DEBUG
        /// TEST: Compare CPU and GPU outputs
        std::cout << "\n[ TEST ] CPU and GPU results equal: ";
        std::cout << compareMatrices(mat_eval, mat_out) ? "PASSED\n" : "FAILED\n";
#endif

        // Print results
        auto total_time_ns = my_program.getTotalKernelTime();
        auto kernel_time_ns = my_program.getKernelExecutionTime();

        std::cout << std::showpoint
                << "\nTime: " << static_cast<double>(kernel_time_ns[0]) / 1000000.0 << '\t'
                << static_cast<double>(total_time_ns[0]) / 1000000.0 << std::noshowpoint << " \n";
        
        // Save matrix
        mat_out = remove_padding(mat_out, resulting_height, resulting_width);
        save_matrix_to_file(mat_out, out_filename);

    } catch(const std::exception& e) {
        std::cerr << "[ Error ]: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "[ Error ]: Unknown error\n";
        return EXIT_FAILURE;
    }

    return 0;
}