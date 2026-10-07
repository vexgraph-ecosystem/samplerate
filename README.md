# samplerate — R5 bare-metal DAW engine layer

## CLion: CMake is IDE metadata only

Open this repository root as a CMake project. `CMakeLists.txt` is an IDE-only
blueprint entry: there are no production sources or C23 source targets yet,
so there is nothing to provide semantic diagnostics or inlay hints for.
No fake declarations, dependency downloads, linking or application runner are
wired into it. IDE appearance is user-verified.

Future builds belong to [b](https://github.com/vex-graph/b). No runnable audio
target or standalone runtime build is claimed by this metadata entry.

**Role:** R5 Interactable — realtime mixer, spatial audio, 3D HRTF. The DSP
engine underneath the `impedance` workstation (`../../../projects/impedance`).
**Status:** stub (LICENSE only; no engine code yet).

## What it is
`samplerate` is the audio counterpart to `anti`/`semicolon`: a from-scratch
digital audio workstation engine in C23 — lock-free realtime mixing,
convolution, spatial/HRTF panning — with zero steady-state allocation on the
audio thread. UI lives in `impedance`; this repo is sound only.

## Depends on (Vertical Integration Law allowlist)
Borrows shapes from R1–R4 (arenas, windows, GPU, UI) to build; owns no
OS/window/memory management itself.

## Layout
- Engine (future): `src/` — mixer, graph, spatializer, HAL glue via `vexspoke` audio.
- Tests: the shared `tests/` repo will host a `tests/samplerate/` partition
  (mirrored per unit, the Test Tree Mirror Law); no test file lives inside this
  repo's source directories (the Test Segregation Law).

## Laws that govern work here
- Constitution: the universal [`preferences.md`](../../vexspoke/preferences.md) (canonical file at `ecosystem/vexspoke/preferences.md`; the workspace root links to it).
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- Bounded Wait Law is load-bearing here: no unbounded waits on the audio path,
  ever — drop-degrade, keep the old buffer, move on.
