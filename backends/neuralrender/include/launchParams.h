#pragma once
#include <cuda_runtime.h>



struct LaunchParams {
    unsigned int frameIndex;
    unsigned int width;
    unsigned int height;
    float4 *frameBuffer;
    
};
