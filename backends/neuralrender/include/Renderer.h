#pragma once

#include <cuda.h>
#include <optix.h>

#include <vector>

#include "launchParams.h"

class Renderer {
public:
    Renderer(
        OptixDeviceContext optixContext,
        unsigned int width,
        unsigned int height
    );

    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void render();
    void downloadFramebuffer(std::vector<float4>& pixels);

    CUdeviceptr framebuffer() const { return d_framebuffer_; }

private:
    void createModule();
    void createProgramGroups();
    void createPipeline();
    void createSBT();
    void allocateBuffers();

    OptixDeviceContext optixContext_ = nullptr;

    OptixModule module_ = nullptr;
    OptixPipelineCompileOptions pipelineCompileOptions_ = {};

    OptixProgramGroup raygenProgramGroup_ = nullptr;
    OptixProgramGroup missProgramGroup_ = nullptr;

    OptixPipeline pipeline_ = nullptr;

    OptixShaderBindingTable sbt_ = {};

    LaunchParams launchParams_ = {};

    CUdeviceptr d_launchParams_ = 0;
    CUdeviceptr d_framebuffer_ = 0;

    CUdeviceptr d_raygenRecord_ = 0;
    CUdeviceptr d_missRecord_ = 0;

    unsigned int width_ = 0;
    unsigned int height_ = 0;
};