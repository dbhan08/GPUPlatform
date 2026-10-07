#include "Renderer.h"
#include "errorCheck.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>



struct __align__(OPTIX_SBT_RECORD_ALIGNMENT) SbtRecord {
    char header[OPTIX_SBT_RECORD_HEADER_SIZE];
};
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

std::vector<char> load_optix_ir_module() {
    // Open the OptiX IR file as a binary stream and jump to the end
    std::ifstream ir_file(NEURALRENDER_OPTIX_IR, std::ios::binary | std::ios::ate);

    // Check if the file opened successfully
    if (!ir_file.is_open()) {
        std::cerr << "ERROR: Failed to open OptiX IR file at: " 
                  << NEURALRENDER_OPTIX_IR << std::endl;
        std::exit(1);
    }

    // Determine file size from the current stream position
    std::streamsize file_size = ir_file.tellg();

    // Seek back to the beginning of the file
    ir_file.seekg(0, std::ios::beg);

    // Allocate the buffer
    std::vector<char> optix_ir_buffer(file_size);

    // Read the data into the buffer
    if (!ir_file.read(optix_ir_buffer.data(), file_size)) {
        std::cerr << "ERROR: Failed to read OptiX IR file content." << std::endl;
        std::exit(1);
    }

    // Stream closes automatically via RAII, but closing explicitly is clean
    ir_file.close();

    return optix_ir_buffer;
}

Renderer::~Renderer() {
    if (d_framebuffer_)   CU_CHECK(cuMemFree(d_framebuffer_));
    if (d_launchParams_)  CU_CHECK(cuMemFree(d_launchParams_));
    if (d_raygenRecord_)  CU_CHECK(cuMemFree(d_raygenRecord_));
    if (d_missRecord_)    CU_CHECK(cuMemFree(d_missRecord_));

    if (pipeline_)            OPTIX_CHECK(optixPipelineDestroy(pipeline_));
    if (raygenProgramGroup_)  OPTIX_CHECK(optixProgramGroupDestroy(raygenProgramGroup_));
    if (missProgramGroup_)    OPTIX_CHECK(optixProgramGroupDestroy(missProgramGroup_));
    if (module_)              OPTIX_CHECK(optixModuleDestroy(module_));
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
    pipelineCompileOptions_ = {};
    pipelineCompileOptions_.usesMotionBlur                   = 0;
    pipelineCompileOptions_.traversableGraphFlags            = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
    pipelineCompileOptions_.numPayloadValues                 = 0;
    pipelineCompileOptions_.numAttributeValues               = 0;
    pipelineCompileOptions_.exceptionFlags                   = OPTIX_EXCEPTION_FLAG_NONE;
    pipelineCompileOptions_.pipelineLaunchParamsVariableName = "optixLaunchParams";
    pipelineCompileOptions_.usesPrimitiveTypeFlags           = OPTIX_PRIMITIVE_TYPE_FLAGS_CUSTOM;

    OptixModuleCompileOptions moduleCompileOptions = {};
    moduleCompileOptions.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
    moduleCompileOptions.optLevel         = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
    moduleCompileOptions.debugLevel       = OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL;

    std::vector<char> optixIr = load_optix_ir_module();

    char   log[2048];
    size_t logSize = sizeof(log);

    OPTIX_CHECK(optixModuleCreate(
        optixContext_,
        &moduleCompileOptions,
        &pipelineCompileOptions_,
        optixIr.data(),
        optixIr.size(),
        log,
        &logSize,
        &module_
    ));

    if (logSize > 1) {
        std::cout << "OptiX module compile log:\n" << log << std::endl;
    }
}


void Renderer::createProgramGroups() {
    OptixProgramGroupOptions programGroupOptions = {};
    OptixProgramGroupDesc raygenDesc = {};
    raygenDesc.kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
    raygenDesc.raygen.module = module_;
    raygenDesc.raygen.entryFunctionName = "__raygen__renderFrame";
    char log[2048] = {};
    size_t logSize = sizeof(log);

    OPTIX_CHECK(optixProgramGroupCreate(
        optixContext_,
        &raygenDesc,
        1,
        &programGroupOptions,
        log,
        &logSize,
        &raygenProgramGroup_
    ));

    if (logSize > 1) {
        std::cout << "Raygen program group log:\n" << log << std::endl;
    }

    OptixProgramGroupDesc missDesc = {};
    missDesc.kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
    missDesc.miss.module = module_;
    missDesc.miss.entryFunctionName = "__miss__radiance";

    logSize = sizeof(log);

    OPTIX_CHECK(optixProgramGroupCreate(
        optixContext_,
        &missDesc,
        1,
        &programGroupOptions,
        log,
        &logSize,
        &missProgramGroup_
    ));

    if (logSize > 1) {
        std::cout << "Miss program group log:\n" << log << std::endl;
    }

}


void Renderer::createPipeline() {
    OptixProgramGroup programGroups[] = {
        raygenProgramGroup_,
        missProgramGroup_
    };

    OptixPipelineLinkOptions linkOptions = {};
    linkOptions.maxTraceDepth = 1;

    char log[2048] = {};
    size_t logSize = sizeof(log);

    OPTIX_CHECK(optixPipelineCreate(
        optixContext_,
        &pipelineCompileOptions_,
        &linkOptions,
        programGroups,
        2,
        log,
        &logSize,
        &pipeline_
    ));
}


void Renderer::createSBT() {
    SbtRecord raygenRecord = {};
    SbtRecord missRecord   = {};

    OPTIX_CHECK(optixSbtRecordPackHeader(raygenProgramGroup_, &raygenRecord));
    OPTIX_CHECK(optixSbtRecordPackHeader(missProgramGroup_, &missRecord));

    CU_CHECK(cuMemAlloc(&d_raygenRecord_, sizeof(SbtRecord)));
    CU_CHECK(cuMemAlloc(&d_missRecord_, sizeof(SbtRecord)));

    CU_CHECK(cuMemcpyHtoD(d_raygenRecord_, &raygenRecord, sizeof(SbtRecord)));
    CU_CHECK(cuMemcpyHtoD(d_missRecord_, &missRecord, sizeof(SbtRecord)));

    sbt_.raygenRecord = d_raygenRecord_;

    sbt_.missRecordBase = d_missRecord_;
    sbt_.missRecordStrideInBytes = sizeof(SbtRecord);
    sbt_.missRecordCount = 1;
}


void Renderer::downloadFramebuffer(std::vector<float4>& pixels) {
    pixels.resize(static_cast<size_t>(width_) * height_);

    CU_CHECK(cuMemcpyDtoH(
        pixels.data(),
        d_framebuffer_,
        pixels.size() * sizeof(float4)
    ));
}


void Renderer::render() {
    CU_CHECK(cuMemcpyHtoD(
        d_launchParams_,
        &launchParams_,
        sizeof(LaunchParams)
    ));

    OPTIX_CHECK(optixLaunch(
        pipeline_,
        0,
        d_launchParams_,
        sizeof(LaunchParams),
        &sbt_,
        width_,
        height_,
        1
    ));

    CU_CHECK(cuCtxSynchronize());

    ++launchParams_.frameIndex;
}
