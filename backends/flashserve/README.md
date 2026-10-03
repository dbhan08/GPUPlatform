# flashserve — LLM Backend

Custom C++/CUDA LLM inference runtime for [GPUPlatform](../../README.md).

**Status: not started.** Deferred until `backends/neuralrender/` reaches its definition of done (see [`../../PLATFORM.md`](../../PLATFORM.md#project-a--flashserve)).

## What it will be

A miniature modern LLM serving stack — decoder-only transformer inference with custom CUDA kernels (RMSNorm, RoPE, SwiGLU, sampling, attention), FP16/BF16 execution, paged KV cache, continuous batching, CUDA Graphs, and an OpenAI-style streaming API. Full spec in `PLATFORM.md`.

## Metrics we care about

tokens/sec · time to first token · inter-token latency · p50/p95 latency · GPU utilization · VRAM · concurrency
