#include "audio/audio_buffer.h"

#include <stdio.h>
#include <string.h>

#include "audio/audio_format.h"
#include "core/audio_memory.h"
#include "oop/type.h"

#include "annotation/definition.h"
#include "annotation/overview.h"
#include "exception/throw.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: AudioBuffer (audio/audio_buffer.c)
 * ============================================================================
 * A flat interleaved 32-bit-float PCM block. Two arena blocks back one logical
 * buffer: the small AudioBuffer control block and one contiguous sample block,
 * so the class pointer stays stable across a growth while the sample store can
 * be reallocated underneath it. Frames is the valid length; capacity is the
 * allocated frame count. All allocation is cold; the realtime callback only
 * borrows the sample pointer.
 *
 * Overflow is refused, not wrapped: frames * channels * sizeof(float) is
 * computed with an explicit overflow check (the Cold-Strict, Hot-Minimal
 * Validation Law), and a rejected resize leaves the previous buffer intact.
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: AudioBuffer (audio/audio_buffer.c)
 * ============================================================================
 * Flat interleaved f32 PCM block (opaque; fields live here).
 *
 * STRUCT FIELDS:
 * ----------------------------------------------------------------------------
 *   float   *data;       // owned interleaved f32 sample block (arena)
 *   uint32_t frames;     // valid frames (<= capacity)
 *   uint32_t capacity;   // allocated frames
 *   uint32_t channels;   // interleaved channel count
 *   double   sampleRate; // Hz
 *
 * PUBLIC SURFACE (audio/audio_buffer.h):
 *   Constructors: AudioBuffer_0/_1(frames)/_2(frames,ch)/_3(frames,ch,rate)
 *   Core:    AudioBuffer_resize(), AudioBuffer_clear(), AudioBuffer_free()
 *   Access:  AudioBuffer_getConstData/_getData/_getFrames/_getCapacity/
 *            _getChannels/_getSampleRate/_setChannels/_setSampleRate/_isValid
 *   Strings: AudioBuffer_toString(), AudioBuffer_toStringStruct()
 * ============================================================================
 */

typedef struct AudioBuffer {
    float *data;
    uint32_t frames;
    uint32_t capacity;
    uint32_t channels;
    double sampleRate;
} AudioBuffer;

// frames * channels * sizeof(float), with explicit overflow refusal.
static bool audioBufferBytes(uint32_t frames, uint32_t channels, size_t *outBytes) {
    if (outBytes == nullptr || frames == 0u || channels == 0u)
        return false;
    if (frames > AUDIO_BUFFER_FRAMES_MAX)
        return false;
    size_t samples = (size_t) frames * (size_t) channels;
    if (samples / (size_t) channels != (size_t) frames)
        return false;                       // multiplication overflowed
    if (samples > SIZE_MAX / sizeof(float))
        return false;
    *outBytes = samples * sizeof(float);
    return true;
}

static AudioBuffer *audioBufferAllocControl(void) {
    AudioBuffer *self = (AudioBuffer*) AudioMemory_alloc(TYPE_AUDIO_BUFFER, sizeof(AudioBuffer));
    if (self == nullptr)
        return nullptr;
    (*self).data = nullptr;
    (*self).frames = 0u;
    (*self).capacity = 0u;
    (*self).channels = AUDIO_CHANNELS_DEFAULT;
    (*self).sampleRate = AUDIO_RATE_DEFAULT;
    return self;
}

// Allocate the sample store for exactly `frames` at the current channel count.
// Cold path; returns false and preserves state on overflow or allocation
// failure.
static bool audioBufferGrowTo(AudioBuffer *self, uint32_t frames) {
    size_t bytes = 0u;
    if (!audioBufferBytes(frames, (*self).channels, &bytes))
        return false;
    float *block = (float*) AudioMemory_alloc(TYPE_AUDIO_BUFFER, bytes);
    if (block == nullptr)
        return false;
    uint32_t copyFrames = (*self).frames < frames ? (*self).frames : frames;
    if ((*self).data != nullptr && copyFrames > 0u) {
        size_t copyFloats = (size_t) copyFrames * (size_t) (*self).channels;
        memcpy(block, (*self).data, copyFloats * sizeof(float));
    }
    AudioMemory_free((*self).data);
    (*self).data = block;
    (*self).capacity = frames;
    return true;
}

AudioBuffer *AudioBuffer_0(void) {
    AudioBuffer *self = audioBufferAllocControl();
    if (self == nullptr)
        return nullptr;
    if (!audioBufferGrowTo(self, AUDIO_BLOCK_FRAMES_DEFAULT)) {
        AudioMemory_free(self);
        return nullptr;
    }
    (*self).frames = AUDIO_BLOCK_FRAMES_DEFAULT;
    return self;
}

AudioBuffer *AudioBuffer_1(uint32_t frames) {
    AudioBuffer *self = audioBufferAllocControl();
    if (self == nullptr)
        return nullptr;
    if (!audioBufferGrowTo(self, frames)) {
        AudioMemory_free(self);
        return nullptr;
    }
    (*self).frames = frames;
    return self;
}

AudioBuffer *AudioBuffer_2(uint32_t frames, uint32_t channels) {
    AudioBuffer *self = AudioBuffer_1(frames);
    if (self != nullptr && channels >= 1u && channels <= AUDIO_CHANNELS_MAX)
        AudioBuffer_setChannels(self, channels);
    return self;
}

AudioBuffer *AudioBuffer_3(uint32_t frames, uint32_t channels, double sampleRate) {
    AudioBuffer *self = AudioBuffer_2(frames, channels);
    if (self != nullptr && sampleRate >= AUDIO_RATE_MIN && sampleRate <= AUDIO_RATE_MAX)
        (*self).sampleRate = sampleRate;
    return self;
}

void AudioBuffer_free(AudioBuffer *self) {
    if (self == nullptr)
        return;
    AudioMemory_free((*self).data);
    (*self).data = nullptr;
    AudioMemory_free(self);
}

bool AudioBuffer_resize(AudioBuffer *self, uint32_t frames) {
    if (self == nullptr) {
        THROW("AudioBuffer_resize: null self");
        return false;
    }
    if (frames == 0u) {
        (*self).frames = 0u;
        return true;
    }
    if (frames > AUDIO_BUFFER_FRAMES_MAX) {
        THROW("AudioBuffer_resize: frames out of range");
        return false;
    }
    if (frames > (*self).capacity) {
        if (!audioBufferGrowTo(self, frames)) {
            THROW("AudioBuffer_resize: growth failed");
            return false;
        }
    }
    (*self).frames = frames;
    return true;
}

void AudioBuffer_clear(AudioBuffer *self) {
    if (self == nullptr || (*self).data == nullptr)
        return;
    size_t floats = (size_t) (*self).frames * (size_t) (*self).channels;
    memset((*self).data, 0, floats * sizeof(float));
}

const float *AudioBuffer_getConstData(const AudioBuffer *self) {
    return self ? (*self).data : nullptr;
}

float *AudioBuffer_getData(AudioBuffer *self) {
    return self ? (*self).data : nullptr;
}

uint32_t AudioBuffer_getFrames(const AudioBuffer *self) {
    return self ? (*self).frames : 0u;
}

uint32_t AudioBuffer_getCapacity(const AudioBuffer *self) {
    return self ? (*self).capacity : 0u;
}

uint32_t AudioBuffer_getChannels(const AudioBuffer *self) {
    return self ? (*self).channels : 0u;
}

double AudioBuffer_getSampleRate(const AudioBuffer *self) {
    return self ? (*self).sampleRate : 0.0;
}

void AudioBuffer_setChannels(AudioBuffer *self, uint32_t channels) {
    if (self == nullptr) {
        THROW("AudioBuffer_setChannels: null self");
        return;
    }
    if (channels == 0u || channels > AUDIO_CHANNELS_MAX) {
        THROW("AudioBuffer_setChannels: channels out of range");
        return;
    }
    uint32_t oldChannels = (*self).channels;
    if (channels == oldChannels)
        return;
    float *block = nullptr;
    if ((*self).capacity > 0u) {
        size_t bytes = 0u;
        if (!audioBufferBytes((*self).capacity, channels, &bytes)) {
            THROW("AudioBuffer_setChannels: overflow");
            return;
        }
        block = (float*) AudioMemory_alloc(TYPE_AUDIO_BUFFER, bytes);
        if (block == nullptr) {
            THROW("AudioBuffer_setChannels: allocation failed");
            return;
        }
        if ((*self).data != nullptr && (*self).frames > 0u) {
            uint32_t copyChannels = oldChannels < channels ? oldChannels : channels;
            for (uint32_t f = 0u; f < (*self).frames; ++f) {
                float *dst = &block[(size_t) f * channels];
                const float *src = &(*self).data[(size_t) f * oldChannels];
                for (uint32_t c = 0u; c < copyChannels; ++c)
                    dst[c] = src[c];
            }
        }
    }
    AudioMemory_free((*self).data);
    (*self).data = block;
    (*self).channels = channels;
}

void AudioBuffer_setSampleRate(AudioBuffer *self, double sampleRate) {
    if (self == nullptr) {
        THROW("AudioBuffer_setSampleRate: null self");
        return;
    }
    if (!(sampleRate >= AUDIO_RATE_MIN) || sampleRate > AUDIO_RATE_MAX) {
        THROW("AudioBuffer_setSampleRate: rate out of range");
        return;
    }
    (*self).sampleRate = sampleRate;
}

bool AudioBuffer_isValid(const AudioBuffer *self) {
    if (self == nullptr)
        return false;
    if ((*self).data == nullptr || (*self).capacity == 0u)
        return false;
    if ((*self).channels < 1u || (*self).channels > AUDIO_CHANNELS_MAX)
        return false;
    if ((*self).frames > (*self).capacity || (*self).frames > AUDIO_BUFFER_FRAMES_MAX)
        return false;
    return (*self).sampleRate >= AUDIO_RATE_MIN && (*self).sampleRate <= AUDIO_RATE_MAX;
}

void AudioBuffer_toString(const AudioBuffer *self, char *dest, size_t cap, bool *outTruncated) {
    if (outTruncated)
        *outTruncated = false;
    if (dest == nullptr || cap == 0u) {
        if (outTruncated)
            *outTruncated = true;
        return;
    }
    if (self == nullptr) {
        int n = snprintf(dest, cap, "nullptr");
        if (outTruncated && n >= 0 && (size_t) n >= cap)
            *outTruncated = true;
        return;
    }
    int n = snprintf(dest, cap, "AudioBuffer(%u/%u frames, %u ch, %.1f Hz)",
                     (*self).frames, (*self).capacity, (*self).channels, (*self).sampleRate);
    if (outTruncated && n >= 0 && (size_t) n >= cap)
        *outTruncated = true;
}

void AudioBuffer_toStringStruct(const AudioBuffer *self, char *dest, size_t cap, bool *outTruncated) {
    if (outTruncated)
        *outTruncated = false;
    if (dest == nullptr || cap == 0u) {
        if (outTruncated)
            *outTruncated = true;
        return;
    }
    if (self == nullptr) {
        int n = snprintf(dest, cap, "nullptr");
        if (outTruncated && n >= 0 && (size_t) n >= cap)
            *outTruncated = true;
        return;
    }
    int n = snprintf(dest, cap,
                     "AudioBuffer { data: %p, frames: %u, capacity: %u, channels: %u, sampleRate: %.1f }",
                     (const void*) (*self).data, (*self).frames, (*self).capacity,
                     (*self).channels, (*self).sampleRate);
    if (outTruncated && n >= 0 && (size_t) n >= cap)
        *outTruncated = true;
}
