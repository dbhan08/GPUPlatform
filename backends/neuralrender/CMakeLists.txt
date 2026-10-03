cmake_minimum_required(VERSION 3.24)
project(neuralrender LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 20)

find_package(CUDAToolkit REQUIRED)

add_executable(neuralrender
    src/main.cpp
    src/cudaContext.cpp
    src/optixContext.cpp
)


target_include_directories(neuralrender PRIVATE
    src/include
    "C:/ProgramData/NVIDIA Corporation/OptiX SDK 9.1.0/include"
)


target_link_libraries(neuralrender PRIVATE
    CUDA::cuda_driver
    CUDA::cudart
)