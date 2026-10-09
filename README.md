# samplerate — R3 native audio driver (engine under the R5 impedance DAW)

## CLion: CMake is IDE metadata only

Open this repository root as a CMake project. `CMakeLists.txt` is an IDE-only
adapter: it provides an excluded C23/Objective-C object target for navigation,
diagnostics and inlay hints, using local `VEXSPOKE_SOURCE_DIR` and
`RELATIONAL_ENGINE_SOURCE_DIR` header paths. Targets are excluded from the
default build; no fake declarations, dependency downloads, linking or
application runner are wired into it. Missing headers stay real IDE errors.
IDE appearance is user-verified.

Actual builds belong to [b](https://github.com/vex-graph/b). No standalone
runtime build or hardware proof is claimed by this metadata entry.

## Current State

**Role:** R3 native audio driver — CoreAudio/WASAPI/ALSA device output, PCM
buffers, the synth/effect DSP graph and offline render/export. It is the engine
under the R5 `impedance` DAW, not an application itself.

**Implemented:** the first R3 slice — `AudioFormat`, `AudioBuffer` and the
`AudioStream` device seam (`platform/audio_stream_cocoa.m`, a default-output
AudioUnit on macOS; a fail-closed stub elsewhere), allocating from a dedicated
Relational Engine arena (`core/audio_memory`). The translation units compile
under `-std=gnu23 -Wall -Wextra -Werror`, and an offline smoke run proves format
validation, arena allocation, buffer growth/overflow refusal and the truncation
flag. No owner test is wired yet and no audio hardware is proven.

**Not yet implemented:** the synth/filter/mixer graph, sequencing, the music
language, offline export/decoding, and the lockless ring transport — all still
specification.

**Platforms proven:** none at the hardware level. macOS code compiles; real
CoreAudio output, Windows WASAPI and Linux ALSA are unproven.

## What it is
`samplerate` is the audio counterpart to `graphvex` (R3 GPU): a from-scratch
native audio driver in C23 — device output, lock-free realtime mixing,
convolution, spatial/HRTF panning and offline render/export — with zero
steady-state allocation on the audio thread. The DAW UI lives in `impedance`
(R5); this repo is sound only.

## Depends on (Vertical Integration Law allowlist)
R3 driver: borrows `vexspoke` and/or `relational-engine` public contracts, plus
`graphvex` for GPU-backed audio compute; its own native CoreAudio/WASAPI/ALSA
backend lives in-repo. Never R1/R4/R5 headers. The DAW application `impedance`
(R5) consumes it.

R2 is split between Vexspoke CPU computation, synchronization and behavior and
Relational Engine memory/storage, stable rows, bindings and native C search.
Migration is staged: existing Vexspoke memory/container ABI and default allocator
remain until explicit migration and owner proof. R1 owns lifetimes/residency;
GPU shaders/dispatch remain Graphvex R3, including any future GPU DSP. No C/Rust
atomic-layout compatibility, automatic schema migration or audio integration is
implied. This audio driver and the R5 application suite are unfinished.

## Layout
- `src/oop/type.h` — the samplerate class registry (`PROJ_SAMPLERATE`).
- `src/core/` — `audio_memory` (dedicated RE arena + transient scratch).
- `src/audio/` — `audio_format`, `audio_buffer` (flat interleaved f32 PCM).
- `src/stream/` — `audio_stream` contract; `src/platform/` its native backends.
- Future: `sample/`, `transport/`, `graph/`, `synth/`, `filter/`, `effect/`,
  `io/`, `seq/`, `music/`, `gpu/`.
- Tests: the shared `../../../tests` repo will host `tests/samplerate/`
  (mirrored per unit, the Test Tree Mirror Law); no test file lives inside this
  repo's source directories (the Test Segregation Law).

## Laws that govern work here
- Constitution: the [canonical preferences.md Gist](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a); one real, Git-ignored workspace-root `../../../preferences.md`, not a Vexspoke file or symlink.
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- Bounded Wait Law is load-bearing here: no unbounded waits on the audio path,
  ever — drop-degrade, keep the old buffer, move on.

## Scope and Limitations

**Scope (intended):** R3 native audio driver — lockless realtime mixing, convolution
and spatial/HRTF panning with zero steady-state allocation on the audio thread,
plus native device output and offline render/export; the DAW UI lives in
`impedance` (R5).

**Deliberately not covered:** no OS/window/memory management (borrowed from
R1–R4); GPU DSP remains Graphvex R3; it never owns host or consumer headers.

**Known limits and gaps:** only the format/buffer/stream slice exists; the DSP
graph, synthesis, sequencing, music language and export are specification. No
audio hardware is proven and no `tests/samplerate/` partition exists yet.
