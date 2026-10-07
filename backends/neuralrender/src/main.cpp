#include "cudaContext.h"
#include "optixContext.h"
#include "Window.h"
#include "Renderer.h"

#include <vector>


int main() {
    CudaContext cuda;
    OptixContext optix(cuda.context());
    Window window(1280, 720, "NeuralRender");
    Renderer renderer(optix.context(), 1280, 720);

    std::vector<float4> pixels;

    while (!window.shouldClose()) {
        renderer.render();
        renderer.downloadFramebuffer(pixels);
        window.display(
            reinterpret_cast<const float*>(pixels.data()),
            1280,
            720
        );
        window.pollEvents();
    }

    return 0;
}
