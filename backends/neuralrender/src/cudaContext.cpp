#include "cudaContext.h"

#include <cstdlib>
#include <iostream>


#define CU_CHECK(call)                                      \
    do {                                                    \
        CUresult result = call;                             \
        if (result != CUDA_SUCCESS) {                       \
            const char* name = nullptr;                     \
            const char* message = nullptr;                  \
            cuGetErrorName(result, &name);                  \
            cuGetErrorString(result, &message);             \
            std::cerr << "CUDA Driver error: "              \
                      << (name ? name : "?")                 \
                      << " - "                               \
                      << (message ? message : "?")           \
                      << std::endl;                          \
            std::exit(1);                                   \
        }                                                   \
    } while (0)


CudaContext::CudaContext() {
    CU_CHECK(cuInit(0));


    int deviceCount = 0;
    CU_CHECK(cuDeviceGetCount(&deviceCount));

    if (deviceCount == 0) {
        std::cerr << "No CUDA devices found" << std::endl;
        std::exit(1);
    }

    std::cout << "CUDA devices found: "
              << deviceCount
              << std::endl;

    CU_CHECK(cuDeviceGet(&device_, 0));

    char deviceName[256];
    CU_CHECK(cuDeviceGetName(deviceName, sizeof(deviceName), device_));

    int major = 0;
    int minor = 0;
    CU_CHECK(cuDeviceGetAttribute(&major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, device_));
    CU_CHECK(cuDeviceGetAttribute(&minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, device_));

    size_t totalMem = 0;
    CU_CHECK(cuDeviceTotalMem(&totalMem, device_));

    std::cout << "GPU 0: " << deviceName
              << "  (sm_" << major << minor
              << ", " << (totalMem >> 20) << " MB)"
              << std::endl;

    CU_CHECK(cuCtxCreate(&context_, nullptr, 0, device_));
}

CudaContext::~CudaContext() {
    if (context_) {
        cuCtxDestroy(context_);
    }
}
