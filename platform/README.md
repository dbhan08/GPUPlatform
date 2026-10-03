# platform — Integration Layer

The GPU serving fabric that fronts both backends in [GPUPlatform](../README.md).

**Status: not started.** Deferred until both backends have working baselines. See [`../PLATFORM.md`](../PLATFORM.md#project-c--shared-gpu-serving-platform).

## Planned components

| Folder         | Role                                                       |
|----------------|------------------------------------------------------------|
| `gateway/`     | API gateway — `POST /generate` (LLM), `POST /render` (gfx) |
| `control/`     | Control plane — job queue, model/scene registries, storage |
| `scheduler/`   | GPU scheduler — SLO-aware, GPU-memory-budgeted             |

## The systems question this exists to answer

> How should a GPU runtime schedule heterogeneous AI workloads with different latency, memory, and compute characteristics?

Example workload targets:
```text
LLM token latency target: < 30 ms
Graphics frame target:    < 16.67 ms  (60 FPS)
```
