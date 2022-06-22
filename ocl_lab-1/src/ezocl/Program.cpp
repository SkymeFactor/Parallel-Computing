#include "ezocl_core.h"



namespace ezocl {


void Program::execute(std::string build_options) {
    std::cout << "[ INFO ]: " << device.get_device_name() << ": " << device.get_device_vendor() << '\n';
    cl_int ret;
    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)device.get_platform_handle(), 0 };

    context = clCreateContextFromType(properties, device.get_cl_device_type(), NULL, NULL, &ret);
    if (ret != CL_SUCCESS) throw Error("Unable to create context");

    command_queue = clCreateCommandQueue(context, device.get_device_handle(), CL_QUEUE_PROFILING_ENABLE, &ret);
    if (ret != CL_SUCCESS) throw Error("Unable to create command_queue");

    const char* src = program_source.c_str();
    program = clCreateProgramWithSource(context, 1, &src, NULL, &ret);
    if (ret != CL_SUCCESS) throw Error("Unable to create program");

    ret = clBuildProgram(program, 0, NULL, build_options.c_str(), NULL, NULL);
    
#ifdef DEBUG
    char var[1024];
    clGetProgramBuildInfo(program, device.get_device_handle(), CL_PROGRAM_BUILD_LOG, 1024, var, NULL);
    std::cout << var << '\n';
#endif
    
    if (ret != CL_SUCCESS) throw Error("Unable to build program");

    

    for (auto& it: kernels) {
        it.enqueueArgs(context, command_queue);
        it.acquireHandle(program);
        it.setArgsToKernel();
        it.runKernel(command_queue);
        kernel_execution_time.push_back(it.getExecutionTime());
    }
    clFinish(command_queue);

    teardown();
};


void Program::teardown()  {
    for (auto& it: kernels) { total_execution_time.push_back(it.getTotalTime()); }
    // Second loop is in case of multiple kernels share the same arguments
    for (auto& it: kernels) { it.releaseArgs(command_queue); }
    if (command_queue != 0) { clReleaseCommandQueue(command_queue); command_queue = 0; }
    for (auto& it: kernels) { it.releaseHandle(); }
    if (program != 0) { clReleaseProgram(program); program = 0; }
    if (context != 0) { clReleaseContext(context); context = 0; }
};


}   // end of namespace ezocl