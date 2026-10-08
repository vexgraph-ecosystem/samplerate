# samplerate — R5 bare-metal DAW engine layer

## CLion: CMake is IDE metadata only

Open this repository root as a CMake project. `CMakeLists.txt` is an IDE-only
blueprint entry: there are no production sources or C23 source targets yet,
so there is nothing to provide semantic diagnostics or inlay hints for.
No fake declarations, dependency downloads, linking or application runner are
wired into it. IDE appearance is user-verified.

Future builds belong to [b](https://github.com/vex-graph/b). No runnable audio
target or standalone runtime build is claimed by this metadata entry.

## Current State

**Role:** R5 interactable — realtime mixer, spatial audio and 3D HRTF: the DSP
engine under the `impedance` workstation.

**Implemented and proven:** nothing. This is a **source-free blueprint**:
`README.md`, `CONTRIBUTING.md`, `LICENSE`, `samplerate-preferences.md`,
`.gitignore` and an IDE-only `LANGUAGES NONE` `CMakeLists.txt`. No `src/`, header,
test partition or build target.

**Specified only:** the Zero-Allocation Audio Callback Law and the Lockless Ring
Transport Law (forward contracts with nothing to bind yet), the mixer/graph/
spatializer/HAL layout, and the `impedance` UI split.

**Platforms proven:** none. (The readiness wiki grades some rows above 🟥, which
is inflated relative to this zero-source tree.)

## What it is
`samplerate` is the audio counterpart to `anti`/`semicolon`: a from-scratch
digital audio workstation engine in C23 — lock-free realtime mixing,
convolution, spatial/HRTF panning — with zero steady-state allocation on the
audio thread. UI lives in `impedance`; this repo is sound only.

## Depends on (Vertical Integration Law allowlist)
Borrows shapes from R1–R4 (arenas, windows, GPU, UI) to build; owns no
OS/window/memory management itself.

R2 is split between Vexspoke CPU computation, synchronization and behavior and
Relational Engine memory/storage, stable rows, bindings and native C search.
Migration is staged: existing Vexspoke memory/container ABI and default allocator
remain until explicit migration and owner proof. R1 owns lifetimes/residency;
GPU shaders/dispatch remain Graphvex R3, including any future GPU DSP. No C/Rust
atomic-layout compatibility, automatic schema migration or audio integration is
implied. This DAW engine and the R5 application suite are unfinished.

## Layout
- Engine (future): `src/` — mixer, graph, spatializer, HAL glue via `vexspoke` audio.
- Tests: the shared `../../../tests` repo will host a `tests/samplerate/` partition
  (mirrored per unit, the Test Tree Mirror Law); no test file lives inside this
  repo's source directories (the Test Segregation Law).

## Laws that govern work here
- Constitution: the [canonical preferences.md Gist](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a); one real, Git-ignored workspace-root `../../../preferences.md`, not a Vexspoke file or symlink.
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- Bounded Wait Law is load-bearing here: no unbounded waits on the audio path,
  ever — drop-degrade, keep the old buffer, move on.

## Scope and Limitations

**Scope (intended):** R5 audio engine — lockless realtime mixing, convolution and
spatial/HRTF panning with zero steady-state allocation on the audio thread; the
UI lives in `impedance`.

**Deliberately not covered:** no OS/window/memory management (borrowed from
R1–R4); GPU DSP remains Graphvex R3; it never owns host or consumer headers.

**Known limits and gaps:** zero implementation — every contract above is
specification only; no platform is proven and no `tests/samplerate/` partition
exists.
