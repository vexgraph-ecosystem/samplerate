# samplerate — Repo-Local Living Preferences
> Repo-local preferences governed by the Living Documentation Law.
> Universal Supreme Constitution: preferences.md (vexspoke).

## 0. Constitution Link (supreme)
- [preferences.md](https://github.com/vexgraph-ecosystem/vexspoke/blob/main/preferences.md) (canonical, vexspoke) — accessible locally at ../../preferences.md
- All universal laws in `../../../preferences.md` are mandatory and binding across the ecosystem.
- This document codifies **exclusive** preferences that apply uniquely to `samplerate` (R4/R5 Audio Engine).

## 1. Repo-Local Law Index (Binding Matrix)

Universal laws are inherited from the canonical `../../../preferences.md` Index; this table indexes the additional laws specific to this repository.

| Law Title | Scope | Enforcement |
| :--- | :--- | :--- |
| **Zero-Allocation Audio Callback Law** | R4/R5 Audio Engine | Mandatory for `samplerate` |
| **Lockless Ring Transport Law** | R4/R5 Audio Engine | Mandatory for `samplerate` |

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

;;INTENTION("R4/R5 Audio Engine: lockless ring buffer transport; zero steady-state allocation in audio callback threads; bounded wait processing.")

---

## 4. Readiness Cross-Reference (Living Documentation Law)

- Feature readiness matrix tracked in [`../../_repositories/.ecosystem/samplerate.md`](../../_repositories/.ecosystem/samplerate.md) (rendered as `[[samplerate]]` wiki page).
