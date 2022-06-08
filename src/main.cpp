#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <chrono>

#include "ezocl_core.h"
#include "utilities.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif



void handle_exception(std::string msg) {
	std::cerr << "[ ERROR ]: " << msg << '\n';
	exit(EXIT_FAILURE);
}


int main(int argc, char* argv[]) {
	if (argc < 4) handle_exception("Invalid number of arguments, 4 required");
	
	// Initialize
	
	std::size_t block_size = LOCAL_SIZE * LOCAL_SIZE * 2;	// Because each thread handles 2 elements

	std::size_t ocl_device_number;
	std::string in_filename {argv[2]};
    std::string out_filename {argv[3]};
	std::string ocl_build_options {"-cl-std=CL1.2 -DBLOCK_SIZE=" + std::to_string(block_size)};
	std::string kernel_filename("src/kernels/prefixSum.cl");
	std::string kernel_name {"prefixSum"};
	std::string kernel_spread_name {"spreadBlockSums"};
	
	try {
        ocl_device_number = static_cast<std::size_t>(std::stoul(argv[1]));
	} catch (const std::exception&) {
		handle_exception("Integral parameter expected, got NaN");
	}

	try {

		// Read vector from file

		auto vec_in = parse_vector_file(argv[2]); // generate_test_data(899);

		auto resulting_size = vec_in.size();
		vec_in = make_zero_padding(vec_in, block_size - vec_in.size() % block_size);

		decltype(vec_in) vec_out;
		decltype(vec_in) block_sums;
		
		vec_out.resize(vec_in.size());
		block_sums.resize(vec_in.size() / block_size);

		// OpenCL set-up
		
		auto ocl_devices = ezocl::DeviceManager::getDevices();
		
		if (ocl_devices.empty()) throw std::runtime_error("No devices found");
        if (ocl_device_number > ocl_devices.size() - 1) ocl_device_number = 0;

		ezocl::Buffer in_buff {&vec_in[0], sizeof(float) * vec_in.size(), ezocl::BufferType::IN_OUT_BUFFER};
		ezocl::Buffer out_buff {&vec_out[0], sizeof(float) * vec_out.size(), ezocl::BufferType::IN_OUT_BUFFER};
		ezocl::Buffer sums_buff {&block_sums[0], sizeof(float) * block_sums.size(), ezocl::BufferType::IN_OUT_BUFFER};
		auto in_buff_shared = std::make_shared<ezocl::Buffer>(in_buff);
		auto out_buff_shared = std::make_shared<ezocl::Buffer>(out_buff);
		auto sums_buff_shared = std::make_shared<ezocl::Buffer>(sums_buff);

		ezocl::Kernel kernel {
			kernel_name,
			ezocl::makeGlobalNDRange({block_size / 2}, vec_in.size() / 2),
			{block_size / 2},
			in_buff_shared, out_buff_shared, sums_buff_shared
		};

		// Time related variables
		std::vector<std::size_t> total_time_ns;
		std::vector<std::size_t> kernel_time_ns;

		// When we have only one block, we don't need spreading
		if (vec_in.size() - block_size != 0){

			ezocl::Kernel kernel_spread {
				kernel_spread_name,
				ezocl::makeGlobalNDRange({block_size / 4}, (vec_in.size() - block_size) / 4),
				{block_size / 4},
				out_buff_shared, sums_buff_shared
			};

			ezocl::Program my_program(kernel_filename, ocl_devices[ocl_device_number], kernel, kernel_spread);
			my_program.execute(ocl_build_options);

			total_time_ns  = my_program.getTotalKernelTime();
			kernel_time_ns  = my_program.getKernelExecutionTime();

		} else {

			ezocl::Program my_program(kernel_filename, ocl_devices[ocl_device_number], kernel);
			my_program.execute(ocl_build_options);

			total_time_ns  = my_program.getTotalKernelTime();
			kernel_time_ns  = my_program.getKernelExecutionTime();
		}
		
		
#ifdef DEBUG
		/// TEST: Compare CPU and GPU outputs
		
		auto t1 = std::chrono::high_resolution_clock::now();
		auto vec_eval = prefixSumCPU(vec_in);
		auto t2 = std::chrono::high_resolution_clock::now();

		std::cout << "\n[ TEST ] CPU and GPU results equal: ";
		std::cout << (compareVectors(vec_eval, vec_out) ? "PASSED\n" : "FAILED\n");
		std::cout << "\nTime: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / 1000000.0);
#endif

		// Print results
		std::cout << std::showpoint
				<< "\nTime: " << (static_cast<double>(kernel_time_ns[0]) +
						static_cast<double>(kernel_time_ns[1])) / 1000000.0 << '\t'
				<< (static_cast<double>(total_time_ns[0]) +
						static_cast<double>(kernel_time_ns[1])) / 1000000.0 << std::noshowpoint << " \n";

		// Save resulting vector to file

		save_vector_to_file(remove_padding(vec_out, vec_out.size() - resulting_size), out_filename);
	
	} catch (const std::exception& e) {
		handle_exception(e.what());
	} catch (...) {
		handle_exception("Unknown error");
	}

	return 0;
}
