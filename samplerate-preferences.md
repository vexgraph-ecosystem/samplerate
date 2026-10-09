# samplerate — Repo-Local Living Preferences
> Repo-local preferences governed by the Living Documentation Law.
> Universal Supreme Constitution: workspace-root preferences.md, published on Gist.

## 0. Constitution Link (supreme)
- [preferences.md](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a) — real, Git-ignored workspace-root file at ../../../preferences.md, not a tracked Vexspoke file or symlink.
- All universal laws in `../../../preferences.md` are mandatory and binding across the ecosystem.
- This document codifies **exclusive** preferences for `samplerate` (R3 native audio driver; sibling to `graphvex`). Vexspoke supplies R2 CPU computation/synchronization/behavior; Relational Engine owns memory/storage/native C search. Its own native CoreAudio/WASAPI/ALSA backend lives in-repo; the R5 DAW application is `impedance`. This blueprint does not replace the current allocator or prove Rust/C audio integration; GPU shaders/dispatch stay Graphvex R3.

## 1. Repo-Local Law Index (Binding Matrix)

Universal laws are inherited from the canonical `../../../preferences.md` Index; this table indexes the additional laws specific to this repository.

| Law Title | Scope | Enforcement |
| :--- | :--- | :--- |
| **Zero-Allocation Audio Callback Law** | R3 Audio Driver | Mandatory for `samplerate` |
| **Lockless Ring Transport Law** | R3 Audio Driver | Mandatory for `samplerate` |

## 2. Exclusive Repo-Local Laws (FULL PROSE RESTATEMENT)

### Zero-Allocation Audio Callback Law

#### Definition:
The real-time audio callback runs in a high-priority OS audio thread and must never execute heap allocation (`malloc`/`free`), system calls, file IO, or mutex locks.

#### The Why:
Audio buffer deadlines are hard real-time constraints; a 5ms delay causes audible glitching and buffer underruns.

#### The Rule:
1. **Zero Malloc:** All audio buffers and DSP state are allocated at stream initialization.
2. **Lock-Free Only:** Inter-thread communication uses wait-free ring buffers.

---

### Lockless Ring Transport Law

#### Definition:
Audio sample packets move between playback/recording devices and DSP processing engines strictly over lockless single-producer single-consumer or multi-producer multi-consumer ring buffers.

#### The Why:
Lock contention between audio render threads and application logic causes priority inversion and dropouts.

#### The Rule:
1. **Atomic Pointers:** Read and write indices are coordinated using atomic memory operations with acquire-release semantics.
2. **Bounded Drift:** Buffer overflow or underflow gracefully fades or silences without deadlock.

---

## 3. Repo-Local Extensions (managed, per the Conflict Triage Law)

;;INTENTION("R3 Audio Driver: lockless ring buffer transport; zero steady-state allocation in audio callback threads; bounded wait processing; native CoreAudio/WASAPI/ALSA backend; the R5 DAW is impedance.")

---

## 4. Readiness Cross-Reference (Living Documentation Law)

- Feature readiness matrix: [samplerate](https://gist.github.com/vex-graph/6943f92acb931b25dad1073c46da6ce7#file-samplerate-md).
- Open blockers and deferred decisions: [ecosystem blockers Gist](https://gist.github.com/vex-graph/e921fa188eebbd0c68c4e59646109887).
