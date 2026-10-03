#include "optixContext.h"

#include <cstdlib>
#include <iostream>

#include <optix_stubs.h>
#include <optix_function_table_definition.h>


#define OPTIX_CHECK(call)                                   \
    do {                                                    \
        OptixResult result = call;                          \
        if (result != OPTIX_SUCCESS) {                      \
            std::cerr << "OptiX error: "                    \
                      << optixGetErrorName(result)           \
                      << " - "                               \
                      << optixGetErrorString(result)         \
                      << std::endl;                          \
            std::exit(1);                                   \
        }                                                   \
    } while (0)


OptixContext::OptixContext(CUcontext cuContext) {
    OPTIX_CHECK(optixInit());

    OptixDeviceContextOptions options = {};
    OPTIX_CHECK(optixDeviceContextCreate(cuContext, &options, &context_));

    std::cout << "OptiX initialized successfully"
              << std::endl;
}

OptixContext::~OptixContext() {
    if (context_) {
        optixDeviceContextDestroy(context_);
    }
}
