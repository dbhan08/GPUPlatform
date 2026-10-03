# GPUPlatform

A GPU serving platform hosting two AI workloads with different latency, memory, and compute profiles:

- **`backends/neuralrender/`** — CUDA/OptiX path tracer + neural denoise/reconstruction/super-res pipeline (real-time graphics under a frame budget)
- **`backends/flashserve/`** — C++/CUDA LLM inference runtime (paged KV cache, continuous batching, CUDA Graphs)
- **`platform/`** — API gateway, control plane, and GPU scheduler that eventually front both backends

The framing question this system exists to answer:

> How should a GPU runtime schedule heterogeneous AI workloads with different latency, memory, and compute characteristics?

The **full plan** — vision, all three components, phase-by-phase renderer roadmap, definition of done, technology stack — lives in [`PLATFORM.md`](./PLATFORM.md). Read that first.

## Status

**Currently building:** `backends/neuralrender/`, Phase 1 (toolchain + application skeleton).

Nothing else is implemented yet. FlashServe and the platform layer are deliberately deferred until the renderer's definition of done is met (see `PLATFORM.md`).

## Target hardware

NVIDIA RTX 5080 (Blackwell, `sm_120`).

## Repository layout

```
GPUPlatform/
├── PLATFORM.md              # full project plan (source of truth)
├── README.md                # this file
├── .gitignore
├── docs/
│   ├── architecture.md      # system diagrams (TBD)
│   └── phases/              # per-phase engineering notes
├── backends/
│   ├── neuralrender/        # Project B — graphics backend  ← current focus
│   │   └── README.md        # renderer-specific 20-phase plan
│   └── flashserve/          # Project A — LLM backend       (later)
│       └── README.md
├── platform/                # Project C — integration layer  (later)
│   ├── gateway/             # API gateway
│   ├── control/             # control plane
│   └── scheduler/           # GPU scheduler
├── common/                  # shared C++/CUDA utilities across backends
└── scripts/                 # dev/build/bench scripts
```

Each subproject has its own `README.md`; this one is the umbrella.

## Environment

Detected on this machine (2026-09-28):

| Tool     | Version                        | Status      |
|----------|--------------------------------|-------------|
| CUDA     | 13.3.73                        | OK          |
| CMake    | 4.4.3                          | OK          |
| Driver   | 610.88 (CUDA UMD 13.3)         | OK          |
| GPU      | RTX 5080, 16 GB, `sm_120`      | OK          |
| MSVC     | —                              | **missing** |
| OptiX    | —                              | **missing** |

Environment install checklist lives in `backends/neuralrender/README.md` (that's the subproject that first needs it).

## Working agreement

- The author drives the code. Claude's role is references, scaffolding, review, and cleanup — not full implementations. See `.claude/` project notes for the details.
- No AI attribution on commits or PRs.
