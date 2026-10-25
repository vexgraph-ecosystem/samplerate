# samplerate — Repo-Local Living Preferences
> Universal constitution: `gist.md` is the compressed law list; `PREFERENCES.md` is its rationale.

## 0. Constitution Link (supreme)
- [gist.md](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a#file-gist-md) — the compressed law list, mirrored in this workspace at `../../../metadata/gists/preferences/gist.md`. It is the only normative law text; `../../../metadata/gists/preferences/PREFERENCES.md` carries the rationale and is never ingested whole. Numbers are slice addresses inside one file, never law identity: cite by Title.
- All universal laws bind all repositories. Read one on demand from the workspace root: `./tools/prefs law "<title>"`, `./tools/prefs section "<section>"`, `./tools/prefs repo <name>`.
- `samplerate`-exclusive: R3 native audio driver, sibling to `graphvex`; Vexspoke supplies R2 CPU computation/synchronization/behavior; Relational Engine owns memory/storage/native C search; native CoreAudio/WASAPI/ALSA backends live here; R5 DAW `impedance` consumes samplerate. No allocator replacement or Rust/C audio integration proof; GPU shaders/dispatch remain Graphvex R3.

## 1. Repo-Local Law Index (Binding Matrix)

Universal laws inherit from the compressed constitution (`../../../metadata/gists/preferences/gist.md`); this table indexes additional repository laws.

| Law Title | Scope | Enforcement |
| :--- | :--- | :--- |
| **Zero-Allocation Audio Callback Law** | R3 Audio Driver | Mandatory for `samplerate` |
| **Lockless Ring Transport Law** | R3 Audio Driver | Mandatory for `samplerate` |

## 2. Exclusive Repo-Local Laws (FULL PROSE RESTATEMENT)

### Zero-Allocation Audio Callback Law

#### Definition:
The high-priority OS real-time audio callback never allocates heap memory (`malloc`/`free`), makes system calls or file IO, or takes mutexes.

#### The Why:
Hard audio deadlines: 5ms delay can cause audible glitching and buffer underruns.

#### The Rule:
1. **Zero Malloc:** Allocate all audio buffers and DSP state at stream initialization.
2. **Lock-Free Only:** Inter-thread communication uses wait-free ring buffers.

---

### Lockless Ring Transport Law

#### Definition:
Playback/recording devices and DSP engines transport audio packets only through lockless single-producer single-consumer (SPSC) or multi-producer multi-consumer (MPMC) ring buffers.

#### The Why:
Lock contention between audio-render threads and application logic causes priority inversion and dropouts.

#### The Rule:
1. **Atomic Pointers:** Coordinate read/write indices with atomic operations using acquire-release semantics.
2. **Bounded Drift:** Overflow or underflow fades or silences gracefully, without deadlock.

---

## 3. Repo-Local Extensions (managed, per the Conflict Triage Law)

;;INTENTION("R3 Audio Driver: lockless ring buffer transport; zero steady-state allocation in audio callback threads; bounded wait processing; native CoreAudio/WASAPI/ALSA backend; the R5 DAW is impedance.")

---

## 4. Readiness Cross-Reference (Living Documentation Law)

- Readiness matrix: [samplerate](https://gist.github.com/vex-graph/6943f92acb931b25dad1073c46da6ce7#file-samplerate-md).
- Open blockers/deferred decisions: [ecosystem blockers Gist](https://gist.github.com/vex-graph/e921fa188eebbd0c68c4e59646109887).
