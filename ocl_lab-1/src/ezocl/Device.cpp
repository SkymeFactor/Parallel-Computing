#include "ezocl_core.h"



namespace ezocl {


void Device::init_properties() {
    cl_bool has_host_unified_memory;
    size_t property_length;
    
    // Get device type
    clGetDeviceInfo(handle, CL_DEVICE_TYPE, sizeof(cl_device_type), &type, NULL);
    clGetDeviceInfo(handle, CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &has_host_unified_memory, NULL);
    
    // Get device name
    clGetDeviceInfo(handle, CL_DEVICE_NAME, 0, NULL, &property_length);
    name.resize(property_length);
    clGetDeviceInfo(handle, CL_DEVICE_NAME, property_length, name.data(), NULL);
    
    // Get vendor name
    clGetDeviceInfo(handle, CL_DEVICE_VENDOR, 0, NULL, &property_length);
    vendor.resize(property_length);
    clGetDeviceInfo(handle, CL_DEVICE_VENDOR, property_length, vendor.data(), NULL);
    
    // Deduce real device type from secondary parameters
    switch (type) {
        case CL_DEVICE_TYPE_CPU: device_type = DeviceType::TYPE_CPU; break;
        case CL_DEVICE_TYPE_GPU:
            has_host_unified_memory ?
                device_type = DeviceType::TYPE_iGPU :
                device_type = DeviceType::TYPE_dGPU;
        break;
        default: break;
    }
};


bool Device::operator<(const Device &rhs) const {
    return device_type < rhs.device_type;
};


}   // end of namespace ezocl