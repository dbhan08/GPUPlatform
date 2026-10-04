cmake_minimum_required(VERSION 3.24)
project(neuralrender LANGUAGES CXX CUDA)

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

add_executable(neuralrender
    src/main.cpp
    src/cudaContext.cpp
    src/optixContext.cpp
    src/Window.cpp
)


target_include_directories(neuralrender PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "C:/ProgramData/NVIDIA Corporation/OptiX SDK 9.1.0/include"
)


target_link_libraries(neuralrender PRIVATE
    CUDA::cuda_driver
    CUDA::cudart
    glfw
)
