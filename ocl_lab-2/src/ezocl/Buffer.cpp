#include "ezocl_core.h"



namespace ezocl {


void Buffer::acquireHandle(cl_context& ctx, cl_command_queue& cq) {
    if (mem_type == BufferType::IN_SCALAR || handle != 0) return;

    cl_int ret = 0;
    handle = clCreateBuffer(ctx, mem_type, size, NULL, &ret);
    if (ret != CL_SUCCESS) throw Error("Unalbe to create buffer");
    
    if (mem_type == BufferType::IN_BUFFER || mem_type == BufferType::IN_OUT_BUFFER) {
        ret = clEnqueueWriteBuffer(cq, handle, CL_FALSE, 0, size, pointer, 0, NULL, &time_profiler[0]);
        if (ret != CL_SUCCESS) throw Error("Unable to enqueue buffer");
    }
};


void Buffer::realeaseHandle(cl_command_queue& cq) {
    if (time_profiler[0] != 0) { clReleaseEvent(time_profiler[0]); time_profiler[0] = 0; }
    if (time_profiler[1] != 0) { clReleaseEvent(time_profiler[1]); time_profiler[1] = 0; }
    if (handle == 0) return;
    
    cl_int ret = 0;
    if (mem_type == BufferType::OUT_BUFFER || mem_type == BufferType::IN_OUT_BUFFER) {
        ret = clEnqueueReadBuffer(cq, handle, CL_TRUE, 0, size, pointer, 0, NULL, &time_profiler[1]);
        if (ret != CL_SUCCESS) throw Error("Unable to read enqueued buffer");
    }

    ret = clReleaseMemObject(handle);
    if (ret != CL_SUCCESS) throw Error("Unable to release buffer");
    handle = 0;
};


std::pair<std::size_t, std::size_t> Buffer::getTransferToDeviceTime() {
    if (time_profiler[0] == 0) return std::make_pair(CL_ULONG_MAX, 0);
    
    cl_int ret = CL_SUCCESS;
    ret |= clWaitForEvents(1, &time_profiler[0]);
    cl_ulong time_start, time_end;

    ret |= clGetEventProfilingInfo(time_profiler[0], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
    ret |= clGetEventProfilingInfo(time_profiler[0], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);
    if (ret != CL_SUCCESS) throw Error("Unable to get profiling info [transfer to]");

    return std::make_pair(time_start, time_end);
};


std::pair<std::size_t, std::size_t> Buffer::getTransferFromDeviceTime() {
    if (time_profiler[1] == 0) return std::make_pair(CL_ULONG_MAX, 0);

    cl_int ret = CL_SUCCESS;
    ret |= clWaitForEvents(1, &time_profiler[1]);
    cl_ulong time_start, time_end;

    ret |= clGetEventProfilingInfo(time_profiler[1], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
    ret |= clGetEventProfilingInfo(time_profiler[1], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);
    if (ret != CL_SUCCESS) throw Error("Unable to get profiling info [transfer from]");
    
    return std::make_pair(time_start, time_end);
};


}   // end of namespace ezocl