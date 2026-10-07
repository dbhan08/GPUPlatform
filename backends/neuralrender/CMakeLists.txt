

cmake_minimum_required(VERSION 3.24)
project(neuralrender LANGUAGES CXX CUDA)

function(compile_optix_ir SOURCE OUTPUT_NAME)

    set(OUTPUT_FILE
        "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME}.optixir"
    )

    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"

        COMMAND "${CMAKE_CUDA_COMPILER}"
            --optix-ir
            --relocatable-device-code=true
            -I"${OPTIX_INCLUDE}"
            -I"${PROJECT_INCLUDE}"
            "${SOURCE}"
            -o "${OUTPUT_FILE}"

        DEPENDS
            "${SOURCE}"
            "${PROJECT_INCLUDE}/launchParams.h"

        COMMENT "Compiling ${OUTPUT_NAME}.optixir"
        VERBATIM
    )

    set(${OUTPUT_NAME}_OPTIX_IR
        "${OUTPUT_FILE}"
        PARENT_SCOPE
    )

endfunction()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 20)

find_package(CUDAToolkit REQUIRED)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

add_library(glad_gl_core_46 STATIC
    third_party/glad/src/gl.c
)

target_include_directories(glad_gl_core_46 PUBLIC
    third_party/glad/include
)

add_executable(neuralrender
    src/main.cpp
    src/cudaContext.cpp
    src/optixContext.cpp
    src/Window.cpp
    src/Renderer.cpp
)

set(OPTIX_INCLUDE
    "C:/ProgramData/NVIDIA Corporation/OptiX SDK 9.1.0/include"
)

set(PROJECT_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(neuralrender PRIVATE
    "${PROJECT_INCLUDE}"
    "${OPTIX_INCLUDE}"
)


compile_optix_ir(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/optix/programs.cu"
    programs
)

add_custom_target(optix_programs
    DEPENDS "${programs_OPTIX_IR}"
)

add_dependencies(neuralrender optix_programs)


target_compile_definitions(neuralrender PRIVATE
    NEURALRENDER_OPTIX_IR="${programs_OPTIX_IR}"
)


target_link_libraries(neuralrender PRIVATE
    CUDA::cuda_driver
    CUDA::cudart
    glfw
    glad_gl_core_46
)
