#include "ezocl_core.h"



namespace ezocl {


// Arguments related methods

void Kernel::enqueueArgs(cl_context& ctx, cl_command_queue& cq) {
    for (auto& it: arguments)
        it->acquireHandle(ctx, cq);
};


void Kernel::releaseArgs(cl_command_queue& cq) {
    for (auto& it: arguments)
        it->realeaseHandle(cq);
};


void Kernel::setArgsToKernel() {
    cl_int ret = CL_SUCCESS;

    for (unsigned i = 0; i < arguments.size(); ++i)
        ret |= clSetKernelArg(handle, i, arguments[i]->getSize(), arguments[i]->getPointer());

    if (ret != CL_SUCCESS) throw Error("Unable to set kernel arguments");
};


// Handle related methods


void Kernel::acquireHandle(cl_program& program_handle) {
    cl_int ret = 0;

    handle = clCreateKernel(program_handle, kernel_name.c_str(), &ret);
    if (ret != CL_SUCCESS) throw Error("Unable to create kernel");
};


void Kernel::releaseHandle() {
    if (time_profiler != 0) { clReleaseEvent(time_profiler); time_profiler = 0; }
    if (handle != 0) { clReleaseKernel(handle); handle = 0; }
};


// Execution related methods


void Kernel::runKernel(cl_command_queue& cq) {
    cl_int ret;
    ret = clEnqueueNDRangeKernel(cq, handle, global_work_size.size(), NULL,
                global_work_size.data(), local_work_size.data(), 0, NULL, &time_profiler);
    if (ret != CL_SUCCESS) throw Error("Unable to enqueue NDRange kernel");
};


// Time measurement related methods


std::size_t Kernel::getExecutionTime() {
    if (time_profiler == 0 || execution_time != 0) return execution_time;

    cl_int ret = CL_SUCCESS;
    ret |= clWaitForEvents(1, &time_profiler);
    cl_ulong time_start = 0, time_end = 0;

    ret |= clGetEventProfilingInfo(time_profiler, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
    ret |= clGetEventProfilingInfo(time_profiler, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);
    if (ret != CL_SUCCESS) throw Error("Unable to get profiling info [kernel]");

    execution_time = static_cast<std::size_t>(time_end - time_start);
    return execution_time;
};


std::size_t Kernel::getTotalTime() {
    std::size_t to_start = CL_ULONG_MAX, to_end = 0;
    std::size_t from_start = CL_ULONG_MAX, from_end = 0;
    
    for (auto& it: arguments) {
        std::pair<std::size_t, std::size_t> temp = it->getTransferToDeviceTime();
        to_start = std::min(temp.first, to_start);
        to_end = std::max(temp.second, to_end);

        temp = it->getTransferFromDeviceTime();
        from_start = std::min(temp.first, from_start);
        from_end = std::max(temp.second, from_end);
    }
    
    return (to_end - to_start) + getExecutionTime() + (from_end - from_start);
};


}   // end of namespace ezocl