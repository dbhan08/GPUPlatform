#include "Renderer.h"
#include "errorCheck.h"


Renderer::Renderer(
    OptixDeviceContext optixContext,
    unsigned int width,
    unsigned int height
) {
    optixContext_ = optixContext;
    width_        = width;
    height_       = height;

    allocateBuffers();
    createModule();
    createProgramGroups();
    createPipeline();
    createSBT();
}

Renderer::~Renderer() {
}


void Renderer::allocateBuffers() {
    const size_t framebufferSize =
        static_cast<size_t>(width_) *
        static_cast<size_t>(height_) *
        sizeof(float4);

    CU_CHECK(cuMemAlloc(&d_framebuffer_, framebufferSize));
    CU_CHECK(cuMemAlloc(&d_launchParams_, sizeof(LaunchParams)));
    launchParams_.width = width_;
    launchParams_.height = height_;
    launchParams_.frameBuffer = reinterpret_cast<float4*>(d_framebuffer_);
    launchParams_.frameIndex = 0;

}


void Renderer::createModule() {
}


void Renderer::createProgramGroups() {
}


void Renderer::createPipeline() {
}


void Renderer::createSBT() {
}


void Renderer::render() {
}
