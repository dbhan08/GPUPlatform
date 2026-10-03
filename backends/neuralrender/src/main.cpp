#include "cudaContext.h"
#include "optixContext.h"
#include "Window.h"


int main() {
    CudaContext cuda;
    OptixContext optix(cuda.context());
    Window window(1280, 720, "NeuralRender");

    while (!window.shouldClose()) {
        window.pollEvents();
    }

    return 0;
}
