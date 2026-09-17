# samplerate — R5 bare-metal DAW engine layer

**Role:** R5 Interactable — realtime mixer, spatial audio, 3D HRTF. The DSP
engine underneath the `impedance` workstation (`projects/impedance`).
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
- Tests: umbrella `tests/` has no `samplerate/` partition yet; until then keep
  seam tests in-repo under `tests/` (never inside source dirs, per the Test
  Segregation Law).

## Laws that govern work here
- Constitution: `../../preferences.md` (umbrella symlink → `ecosystem/vexspoke/preferences.md`).
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- Bounded Wait Law is load-bearing here: no unbounded waits on the audio path,
  ever — drop-degrade, keep the old buffer, move on.
