# neuralrender — Decision Log

Design decisions that influence code shape, scoped by phase. Each entry: context, options considered, chosen path, why, and when to revisit. Entries are append-only; supersede rather than edit.

Format template:

```
## YYYY-MM-DD — Phase N — <short decision title>

**Context.** What triggered the decision.
**Options.** Short bullet list of what was on the table.
**Chosen.** The option taken.
**Why.** The reason the chosen option won.
**Revisit when.** The future event that will make this worth reopening.
```

---

## 2026-10-05 — Phase 2 — Payload transport: direct-write miss, defer PRD/pack-pointer

**Context.** Phase 2's definition of done is "no geometry, every ray misses, miss writes a gradient." The miss program needs to produce a color per pixel; raygen builds the ray. The question was whether to introduce a `PerRayData` struct + pack-pointer payload now (so raygen owns the final framebuffer write) or let the miss program write to the framebuffer directly.

**Options.**
- **A. Direct-write miss.** Miss reads `optixLaunchParams.frameBuffer` and writes the pixel itself. Raygen's `optixTrace` takes zero payload registers.
- **B. PRD + pack-pointer now.** Miss writes `prd->color`; raygen packs a pointer to a stack-allocated `PerRayData` into two payload registers; raygen writes the framebuffer after `optixTrace` returns.

**Chosen.** A — direct-write miss.

**Why.** Phases are the scoping mechanism; adding Phase 5 (multi-bounce path tracing) machinery into Phase 2 defeats the phased roadmap. Direct-write is the smallest shape that satisfies Phase 2's goal, keeps the data flow legible (`raygen → optixTrace → miss → framebuffer`), and the PRD struct will likely need to grow richer than `{ float3 color; }` by the time we actually need it — so pre-wiring it today does not avoid the eventual rewrite.

**Revisit when.** First phase that needs raygen to receive something back from a trace — i.e. the first closest-hit shader with real shading, or when multi-bounce path tracing lands (Phase 5-ish). At that point introduce `PerRayData` with the fields that phase actually needs, not speculative ones.

---

## 2026-10-05 — Phase 2 — Camera parameters stay inline in raygen, no `Camera` substruct yet

**Context.** Debate on whether to add `Camera { position, U, V, W }` to `LaunchParams` now so Phase 3+ is a host-side swap, vs. leave the camera hard-coded in raygen (origin at `(0,0,0)`, direction `(u*aspect, v, -1)`).

**Options.**
- **A. Hard-code in raygen.** Classic temporary camera; direction built from screen coords inline.
- **B. Add `Camera` substruct to `LaunchParams` now.** Host feeds basis each frame; raygen reads `optixLaunchParams.camera`.

**Chosen.** A — hard-code the temporary camera in raygen.

**Why.** The roadmap already schedules a dedicated camera phase. Pulling `Camera { position, U, V, W }` into `LaunchParams` early just to avoid a struct change later means Phase 2 carries machinery that serves no Phase-2 goal. The temporary origin-at-zero/looking-down-Z camera *is* the correct Phase-2 camera.

**Revisit when.** Camera phase (orbit/FPS controls, FOV from host). That is the moment to design the `Camera` struct against real requirements, not speculative ones.

---

## 2026-10-05 — Phase 2 — Traversable handle stays inline `0`, no field in `LaunchParams` yet

**Context.** `optixTrace` needs an `OptixTraversableHandle`. Phase 2 has no acceleration structure, so the handle is `0` (every ray immediately misses). Choice: leave it inline as `static_cast<OptixTraversableHandle>(0)` in the raygen call, or add `OptixTraversableHandle traversable` to `LaunchParams` now (initialized to `0` host-side) so Phase 3 is a host-only swap.

**Options.**
- **A. Inline `0` in raygen.** `LaunchParams` stays free of OptiX types.
- **B. Add `traversable` field now.** Phase 3 only needs the host side to assign the real handle; the device-side `optixTrace` call never changes.

**Chosen.** A — inline `0`.

**Why.** Marginal call, but adding the field means `launchParams.h` (currently CUDA-only via `cuda_runtime.h`) pulls in `optix.h` early. The Phase-3 edit is a one-liner — `0` → `optixLaunchParams.traversable` — not worth the earlier header surface.

**Revisit when.** Phase 3 (first GAS build). Add the field in the same commit that introduces the acceleration structure.

---

## 2026-10-05 — Phase 2 — Output buffer named `frameBuffer`, no G-buffer siblings yet

**Context.** Future phases add `normalBuffer`, `albedoBuffer`, `depthBuffer`, `motionBuffer` for the denoiser input pipeline. Choice today: rename to `colorBuffer` to set vocabulary, or keep `frameBuffer`.

**Options.**
- **A. Keep `frameBuffer`.** One output today, naming captures intent as "the frame shown on screen."
- **B. Rename to `colorBuffer`.** Signals that siblings are coming; aligns with G-buffer phase vocabulary.

**Chosen.** A — keep `frameBuffer`.

**Why.** Same principle: no need to seed Phase 4 vocabulary into Phase 2 code. When G-buffer channels land, rename and introduce siblings together in one phase-coherent commit.

**Revisit when.** G-buffer phase (Phase 4-ish per the renderer roadmap). Rename + add siblings in that phase's first commit.
