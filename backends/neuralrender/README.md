# neuralrender — Graphics Backend

Real-time neural graphics for [GPUPlatform](../../README.md).

A CUDA/OptiX path tracer that produces cheap noisy frames plus aligned G-buffer features (normals, albedo, depth), which a neural denoiser + temporal reconstruction + super-resolution pipeline reconstructs into a high-quality frame under a strict frame budget.

The renderer is the current focus. AI stages (denoiser, TensorRT deployment, temporal reconstruction, super-resolution) come after the renderer's definition of done.

## Status

**Phase 1 — Toolchain & Application Skeleton** (in progress).

The full 20-phase renderer roadmap and definition of done live in [`../../PLATFORM.md`](../../PLATFORM.md#renderer-development-phases). Per-phase notes go under [`../../docs/phases/`](../../docs/phases/).

## Tech stack

- C++17/20
- CUDA (host toolchain: MSVC)
- NVIDIA OptiX
- CMake
- GLFW + OpenGL (initial display path)
- GLM
- OpenEXR (float buffer export, later phase)

## Planned layout

```
neuralrender/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── cuda/          # CUDA kernels
│   ├── optix/         # OptiX device programs (.cu compiled to PTX/OPTIX-IR)
│   └── host/          # CPU-side runtime, window, scheduling
├── shaders/           # OpenGL blit
├── scenes/            # meshes, glTF/OBJ inputs
├── datasets/          # generated aligned (noisy, reference, normal, depth, albedo) — gitignored
├── external/          # third-party (glm, tinyobj/tinygltf, stb, tinyexr)
└── cmake/             # FindOptiX, toolchain helpers
```

## Prerequisites (missing right now)

1. **Visual Studio 2022 Community** — <https://visualstudio.microsoft.com/downloads/>
   - Workload: **Desktop development with C++**
   - Components: MSVC v143 x64 toolset, Windows 11 SDK, CMake tools for Windows
2. **NVIDIA OptiX SDK 9.x** — <https://developer.nvidia.com/rtx/ray-tracing/optix> (NVIDIA developer login required)
   - Default install path: `C:\ProgramData\NVIDIA Corporation\OptiX SDK 9.x.x\`
   - After install, set env var `OPTIX_ROOT` to that path (used by our `FindOptiX.cmake`).

CUDA 13.3, CMake 4.4.3, driver 610.88, RTX 5080 already present — see the platform-level [README](../../README.md#environment).

## Build

_TBD — filled in as Phase 1 lands._
