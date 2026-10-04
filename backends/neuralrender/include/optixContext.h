#pragma once

#include <cuda.h>
#include <optix.h>

class OptixContext {
public:
    explicit OptixContext(CUcontext cuContext);
    ~OptixContext();

    OptixDeviceContext context() const { return context_; }

private:
    OptixDeviceContext context_ = nullptr;
};
