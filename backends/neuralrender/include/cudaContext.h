#pragma once

#include <cuda.h>

class CudaContext {
public:
    CudaContext();
    ~CudaContext();

    CUdevice device() const { return device_; }
    CUcontext context() const { return context_; }

private:
    CUdevice device_ = 0;
    CUcontext context_ = nullptr;
};
