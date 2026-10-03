#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "cudaContext.h"
#include "optixContext.h"


int main() {
    CudaContext cuda;
    OptixContext optix(cuda.context());

    return 0;
}
