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
