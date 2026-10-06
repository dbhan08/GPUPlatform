#pragma once

#include <cuda.h>
#include <optix.h>
#include <optix_stubs.h>

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
