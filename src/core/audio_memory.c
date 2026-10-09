#include "core/audio_memory.h"

#include "annotation/definition.h"
#include "annotation/overview.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: AudioMemory (core/audio_memory.c)
 * ============================================================================
 * The dedicated Relational Engine arena for the whole Samplerate driver. One
 * master arena is created lazily at first use and every long-lived audio
 * object (format, buffer, stream, and the voices/effects to come) is carved
 * from it, so teardown is a single MemoryArena_destroy at the end of the R1
 * shutdown order and never touches the process-global default arena.
 *
 * A separate bump-only transient arena serves per-render-block scratch. Both
 * allocation paths are COLD: the realtime CoreAudio callback reads prepared
 * blocks and never calls in here (the Zero-Allocation Audio Callback Law).
 * Arena creation happens pre-threads; the hot path takes no extra locks.
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: AudioMemory (core/audio_memory.c)
 * ============================================================================
 * Samplerate's dedicated RE arena + transient scratch owner.
 *
 * FILE-STATIC STATE:
 * ----------------------------------------------------------------------------
 *   MemoryArena *s_arena;      // the dedicated Samplerate arena (nullptr = not yet created)
 *   bool         s_scratchReady; // transient scratch initialized once
 *
 * PUBLIC SURFACE (core/audio_memory.h):
 *   - AudioMemory_arena(void)
 *   - AudioMemory_alloc(typeId, numBytes)
 *   - AudioMemory_realloc(userPtr, newBytes)
 *   - AudioMemory_free(userPtr)
 *   - AudioMemory_scratch(typeId, numBytes)
 *   - AudioMemory_resetScratch(void)
 *   - AudioMemory_shutdown(void)
 * ============================================================================
 */

static MemoryArena *s_arena = nullptr;
static bool s_scratchReady = false;

MemoryArena *AudioMemory_arena(void) {
    if (s_arena == nullptr)
        s_arena = MemoryArena_create(AUDIO_MEMORY_DEFAULT_BYTES);
    return s_arena;
}

void *AudioMemory_alloc(uint64_t typeId, size_t numBytes) {
    MemoryArena *a = AudioMemory_arena();
    if (a == nullptr || numBytes == 0)
        return nullptr;
    return MemoryArena_alloc(a, typeId, numBytes);
}

void *AudioMemory_realloc(void *userPtr, size_t newBytes) {
    MemoryArena *a = s_arena;
    if (a == nullptr || userPtr == nullptr || newBytes == 0)
        return nullptr;
    return MemoryArena_realloc(a, userPtr, newBytes);
}

void AudioMemory_free(void *userPtr) {
    if (userPtr == nullptr || s_arena == nullptr)
        return;
    MemoryArena_free(s_arena, userPtr);
}

void *AudioMemory_scratch(uint64_t typeId, size_t numBytes) {
    if (!s_scratchReady)
        s_scratchReady = Memory_initTransient(AUDIO_MEMORY_DEFAULT_SCRATCH_BYTES);
    if (!s_scratchReady || numBytes == 0)
        return nullptr;
    return Transient_alloc(typeId, numBytes);
}

void AudioMemory_resetScratch(void) {
    Transient_reset();
}

void AudioMemory_shutdown(void) {
    if (s_arena != nullptr) {
        MemoryArena_destroy(s_arena);
        s_arena = nullptr;
    }
    s_scratchReady = false;
}
