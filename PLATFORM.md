# Full Project Context

I am building a portfolio of technically deep GPU systems projects focused on **CUDA, AI inference, graphics, and real-time GPU scheduling**.

The overall goal is to demonstrate that I can work across:

- CUDA kernels
- C++ GPU runtimes
- AI inference optimization
- graphics workloads
- memory management
- scheduling
- latency-sensitive serving
- GPU profiling
- systems design
- backend infrastructure

The work is intentionally split into separate projects first so that each one is technically clean and understandable. Later, the projects may be integrated into a shared GPU-serving system.

There are currently three conceptual layers.

---

# Project A — FlashServe

FlashServe is a custom **LLM inference runtime** written in C++/CUDA.

Its long-term goal is to implement a miniature modern LLM serving stack rather than relying entirely on existing systems like vLLM.

The target architecture is roughly:

```text
Client
  ↓
LLM request
  ↓
Request scheduler
  ↓
Continuous batching
  ↓
Paged KV cache
  ↓
CUDA Graph decode
  ↓
Custom CUDA kernels
  ↓
RTX 5080
```

Planned features include:

- decoder-only transformer inference
- custom CUDA kernels for operations such as:
  - RMSNorm
  - RoPE
  - SwiGLU
  - sampling
  - attention
- FP16/BF16 execution
- paged KV-cache management
- continuous batching
- CUDA Graphs
- model memory management
- request scheduling
- OpenAI-style streaming API
- potentially:
  - INT4/FP8
  - speculative decoding
  - prefix caching
  - fused attention
  - distributed inference later

Metrics include:

- tokens/sec
- time to first token
- inter-token latency
- p50/p95 latency
- GPU utilization
- VRAM usage
- concurrency

This is the pure **AI inference systems / CUDA runtime** project.

---

# Project B — NeuralRenderRT

NeuralRenderRT is a separate project focused on **AI + graphics + CUDA**.

The goal is to build a real-time neural graphics system where traditional GPU rendering produces a low-cost noisy image, and AI models reconstruct a high-quality frame.

Conceptually:

```text
3D scene
   ↓
CUDA / OptiX renderer
   ↓
low-sample noisy frame
   ↓
G-buffer features
   ↓
neural denoiser
   ↓
temporal reconstruction
   ↓
neural super-resolution
   ↓
high-quality real-time frame
```

The project is intentionally not just a path tracer and not just an image model.

It should sit at the intersection of:

- graphics
- AI inference
- CUDA
- real-time systems
- neural rendering
- GPU performance engineering

The target machine is an NVIDIA RTX 5080.

The eventual NeuralRenderRT system should include:

- OptiX/CUDA path tracing
- low-sample rendering
- high-sample reference rendering
- G-buffer outputs
- neural denoising
- TensorRT inference
- temporal reconstruction
- neural upscaling
- custom CUDA pre/post-processing
- CUDA Graphs
- CUDA streams
- async execution
- frame-time-aware scheduling
- profiling with Nsight

The eventual performance target is real-time rendering under a strict frame budget, for example:

```text
60 FPS  = 16.67 ms/frame
120 FPS = 8.33 ms/frame
```

The renderer should ultimately become one kind of **AI inference workload**, not a standalone graphics-only project.

---

# Project C — Shared GPU Serving Platform

This is a future integration layer.

FlashServe and NeuralRenderRT may eventually become two execution backends behind a larger GPU service.

The eventual system could look like:

```text
                       Client
                         │
                         ▼
                   API Gateway
                         │
                         ▼
                   Control Plane
                         │
          ┌──────────────┼──────────────┐
          │              │              │
      PostgreSQL       Redis       Object Storage
          │              │              │
          └──────────────┼──────────────┘
                         ▼
                   GPU Scheduler
                         │
              ┌──────────┴──────────┐
              │                     │
              ▼                     ▼
         FlashServe          NeuralRenderRT
         LLM backend         graphics backend
              │                     │
              └──────────┬──────────┘
                         ▼
                    CUDA Runtime
                         │
                         ▼
                       GPU(s)
```

Possible infrastructure features:

- job queue
- async jobs
- model registry
- scene registry
- model/artifact storage
- object storage
- GPU memory budgeting
- model residency management
- model eviction
- admission control
- latency-SLO-aware scheduling
- GPU worker heartbeats
- retries / fault handling
- metrics
- Prometheus
- Grafana
- GPU utilization monitoring
- per-workload latency metrics

Potential API shapes:

```text
POST /generate
```

for LLM inference,

and:

```text
POST /render
```

for neural graphics workloads.

The shared runtime could eventually schedule workloads with different latency requirements.

Example:

```text
LLM token latency target: < 30 ms
Graphics frame target:    < 16.67 ms
```

The research/systems question would be:

> How should a GPU runtime schedule heterogeneous AI workloads with different latency, memory, and compute characteristics?

This future integration is important context, but it should NOT be implemented now.

---

# Current Focus

The current task is **Project B: NeuralRenderRT**.

More specifically, the immediate focus is only the first major component:

# Renderer Foundation

Do not implement FlashServe.

Do not implement the shared serving platform.

Do not implement the neural denoiser yet.

Do not implement TensorRT yet.

Do not implement temporal reconstruction yet.

Do not implement super-resolution yet.

Right now the goal is to build the graphics foundation that will later generate training and inference inputs for the AI system.

---

# Renderer Foundation Goal

Build a GPU path tracer that produces:

- noisy low-sample RGB
- clean high-sample reference RGB
- surface normals
- albedo
- depth
- optional world position
- optional material/object IDs

These outputs must align pixel-for-pixel.

The renderer should support:

```text
1–4 SPP
```

for noisy real-time-ish inputs,

and:

```text
128–512+ SPP
```

for clean reference targets.

The future AI denoiser will learn approximately:

```text
noisy RGB
+ normals
+ depth
+ albedo
      ↓
neural model
      ↓
clean RGB
```

So all current renderer decisions should preserve that future use case.

---

# Renderer Technology Stack

Use:

- C++17 or C++20
- CUDA
- NVIDIA OptiX
- CMake
- GLFW
- OpenGL initially for display
- GLM
- EXR output for floating-point render buffers if practical

Target GPU:

```text
NVIDIA RTX 5080
```

The renderer should be modular enough to integrate later with TensorRT and custom CUDA inference code.

---

# High-Level Renderer Architecture

```text
Scene Loader
    │
    ▼
Geometry / Materials
    │
    ▼
OptiX Acceleration Structures
    │
    ▼
Camera
    │
    ▼
Ray Generation
    │
    ▼
Path Tracing
    │
    ├────────────┬────────────┬────────────┐
    ▼            ▼            ▼            ▼
   RGB         Normal       Albedo        Depth
    │
    ▼
Progressive Accumulation
    │
    ▼
Interactive Display / Dataset Export
```

---

# Renderer Development Phases

## Phase 1 — Toolchain and Application Skeleton

Implement:

- CMake project
- GLFW window
- CUDA initialization
- CUDA device selection
- OptiX initialization
- OptiX context
- CUDA/OptiX error handling
- clean shutdown

Success criteria:

- app builds
- window opens
- RTX 5080 is detected
- CUDA works
- OptiX works
- no rendering yet

---

## Phase 2 — First OptiX Launch

Implement:

- launch parameters
- raygen program
- miss program
- framebuffer
- OptiX module
- program groups
- pipeline
- shader binding table
- `optixLaunch`

No geometry yet.

Every ray should miss and produce a sky/background gradient.

Success criteria:

- GPU-generated image appears
- changing miss shader changes output
- no CPU per-pixel rendering

---

## Phase 3 — First Triangle

Implement:

- triangle vertex buffer
- GAS
- closest-hit program
- hitgroup
- SBT records

Behavior:

```text
triangle hit → color
miss → sky
```

Success criteria:

- first ray-traced triangle renders correctly

---

## Phase 4 — Camera

Add:

- position
- look-at
- up vector
- FOV
- aspect ratio
- camera basis
- pixel-to-ray conversion
- WASD
- mouse look

Success criteria:

- interactive navigation works

---

## Phase 5 — Mesh Loading

Support OBJ or glTF.

Load:

- vertices
- normals
- triangle indices

Upload to GPU and build GAS.

Success criteria:

- arbitrary triangle meshes render

---

## Phase 6 — Surface Normals

Compute:

- barycentric interpolation
- vertex normals
- geometric normals
- world-space normals

Add debug visualization:

```text
RGB = normal * 0.5 + 0.5
```

Create separate normal buffer.

---

## Phase 7 — Materials and Albedo

Start with:

```cpp
struct Material {
    float3 albedo;
};
```

Support basic per-object material colors.

Create separate albedo buffer.

---

## Phase 8 — Depth

Store:

```text
depth = distance(camera, hit_position)
```

Create independent floating-point depth buffer.

---

## Phase 9 — Direct Lighting

Implement:

- point or directional light
- Lambertian shading
- shadow rays

---

## Phase 10 — Area Lighting

Add sampled area lights and soft shadows.

---

## Phase 11 — Path Tracing

Implement:

- hemisphere sampling
- indirect illumination
- 2–4 bounces initially
- Lambertian BRDF

---

## Phase 12 — GPU RNG

Use PCG, Philox, XORWOW, or another appropriate GPU RNG.

Needed for:

- subpixel jitter
- hemisphere sampling
- light sampling

---

## Phase 13 — 1 SPP Mode

Support intentionally noisy:

```text
SPP = 1
```

This becomes the future AI input.

---

## Phase 14 — High-SPP Reference Mode

Support:

```text
64
128
256
512+
```

SPP.

This becomes the training target.

---

## Phase 15 — Progressive Accumulation

Accumulate frames progressively.

Reset when:

- camera moves
- geometry changes
- material changes
- lighting changes

---

## Phase 16 — Formal G-Buffer

Expose separate GPU buffers for:

```text
color
normal
albedo
depth
```

Optional:

```text
world position
object ID
material ID
```

Prefer SoA-style layout unless profiling later suggests otherwise.

---

## Phase 17 — Dataset Export

Add offline capture mode.

Example:

```text
scene_00001/
├── noisy.exr
├── reference.exr
├── normal.exr
├── albedo.exr
├── depth.exr
└── metadata.json
```

Metadata should contain:

- camera pose
- projection parameters
- resolution
- noisy SPP
- reference SPP
- scene identifier

---

## Phase 18 — Performance Baseline

Benchmark:

```text
720p
1080p
1440p
4K
```

with:

```text
1 SPP
2 SPP
4 SPP
```

Record:

- frame time
- FPS
- VRAM
- rays/sec if available
- GPU utilization

---

## Phase 19 — Profiling

Use Nsight Systems and Nsight Compute.

Investigate:

- GPU idle time
- synchronization
- launch overhead
- memory bandwidth
- occupancy
- warp divergence
- cache behavior
- instruction throughput

Do not optimize before collecting baseline measurements.

---

## Phase 20 — Targeted Optimization

Only optimize measured bottlenecks.

Possible targets:

- ray payload size
- SBT organization
- memory layout
- material storage
- divergence
- acceleration-structure settings
- synchronization
- accumulation storage
- host/device transfers

For every optimization, preserve:

```text
before
change
after
```

benchmark evidence.

---

# Renderer Definition of Done

The renderer component is finished when it can:

- load a scene
- ray trace using OptiX
- path trace with indirect illumination
- render interactively
- produce 1-SPP noisy frames
- produce high-SPP references
- output normals
- output depth
- output albedo
- progressively accumulate
- generate aligned AI training datasets
- produce baseline benchmarks
- show profiling evidence

At that point, stop extending the graphics engine temporarily.

---

# What Happens After the Renderer

The next NeuralRenderRT stage will be AI.

## Neural denoiser

Inputs:

```text
noisy RGB
normal
depth
albedo
```

Target:

```text
high-SPP reference RGB
```

Initial model:

- small CNN or U-Net
- PyTorch training

Then deployment:

```text
PyTorch
   ↓
ONNX
   ↓
TensorRT
   ↓
real-time inference
```

Later optimization:

- FP16
- possibly FP8
- custom CUDA preprocessing
- custom CUDA postprocessing
- CUDA Graphs
- neural super-resolution
- temporal reconstruction
- graphics/inference overlap
- adaptive quality scheduler

The final project should eventually answer:

> Can a cheap low-sample GPU render plus neural inference produce a high-quality frame under a strict real-time frame budget?

---

# How to Work on This Project

Treat this as an ongoing engineering project, not a one-shot code-generation task.

For each phase:

1. explain the purpose of the phase
2. identify files to create or modify
3. make the smallest correct implementation
4. preserve previous functionality
5. explain nontrivial architecture choices
6. provide exact build/run commands
7. provide validation steps
8. stop after the phase succeeds
9. do not jump ahead unless required by a dependency

When debugging:

- identify root cause
- distinguish environment/setup issues from code issues
- keep CUDA/OptiX error handling intact
- do not silence errors merely to make the build pass

When optimizing:

- measure first
- preserve baseline numbers
- explain why the optimization should help
- rerun the same benchmark after changes

The code should remain understandable enough that I can explain the architecture and implementation choices in a technical interview.

---

# Immediate Task

Start with **NeuralRenderRT Phase 1 only**.

The machine has an NVIDIA RTX 5080.

Before implementing Phase 1, determine the current CUDA, OptiX, compiler, CMake, and OS environment.

Do not implement Phase 2 until Phase 1 works.
