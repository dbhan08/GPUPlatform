#include "optixContext.h"
#include "errorCheck.h"

#include <optix_function_table_definition.h>


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
