#include "audio/audio_format.h"

#include <stdio.h>

#include "core/audio_memory.h"
#include "oop/type.h"

#include "annotation/definition.h"
#include "annotation/overview.h"
#include "exception/throw.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: AudioFormat (audio/audio_format.c)
 * ============================================================================
 * The negotiated shape of a PCM stream: sample rate, channel count, frames per
 * render block, and sample type. It is a small cold value object carved from
 * the dedicated Samplerate arena (core/audio_memory), stamped with
 * TYPE_AUDIO_FORMAT so Memory_type() can identify it. One instance is handed to
 * an AudioStream and an AudioBuffer at preparation time; the realtime callback
 * never touches it. Rejections (an out-of-range setter) are cold and reported
 * with THROW, never silently clamped, so a bad rate cannot masquerade as good.
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: AudioFormat (audio/audio_format.c)
 * ============================================================================
 * PCM stream-shape value object (opaque; fields live here).
 *
 * STRUCT FIELDS (audio/audio_format.c only):
 * ----------------------------------------------------------------------------
 *   double   sampleRate;      // Hz (AUDIO_RATE_MIN..AUDIO_RATE_MAX)
 *   uint32_t channels;        // 1..AUDIO_CHANNELS_MAX
 *   uint32_t framesPerBlock;  // render quantum in frames (1..AUDIO_BLOCK_FRAMES_MAX)
 *   uint32_t sampleType;      // AUDIO_SAMPLE_*
 *
 * PUBLIC SURFACE (audio/audio_format.h):
 *   Constructors: AudioFormat_0(), AudioFormat_1(rate), AudioFormat_2(rate, ch)
 *   Setters: AudioFormat_setSampleRate/_setChannels/_setFramesPerBlock/_setSampleType
 *   Getters: AudioFormat_getSampleRate/_getChannels/_getFramesPerBlock/_getSampleType
 *   Core:    AudioFormat_isValid(), AudioFormat_free()
 *   Strings: AudioFormat_toString(), AudioFormat_toStringStruct()
 * ============================================================================
 */

typedef struct AudioFormat {
    double sampleRate;
    uint32_t channels;
    uint32_t framesPerBlock;
    uint32_t sampleType;
} AudioFormat;

static AudioFormat *audioFormatAlloc(void) {
    AudioFormat *self = (AudioFormat*) AudioMemory_alloc(TYPE_AUDIO_FORMAT, sizeof(AudioFormat));
    return self;
}

AudioFormat *AudioFormat_0(void) {
    AudioFormat *self = audioFormatAlloc();
    if (self == nullptr)
        return nullptr;
    (*self).sampleRate = AUDIO_RATE_DEFAULT;
    (*self).channels = AUDIO_CHANNELS_DEFAULT;
    (*self).framesPerBlock = AUDIO_BLOCK_FRAMES_DEFAULT;
    (*self).sampleType = AUDIO_SAMPLE_TYPE_DEFAULT;
    return self;
}

AudioFormat *AudioFormat_1(double sampleRate) {
    AudioFormat *self = AudioFormat_0();
    if (self != nullptr)
        AudioFormat_setSampleRate(self, sampleRate);
    return self;
}

AudioFormat *AudioFormat_2(double sampleRate, uint32_t channels) {
    AudioFormat *self = AudioFormat_1(sampleRate);
    if (self != nullptr)
        AudioFormat_setChannels(self, channels);
    return self;
}

void AudioFormat_free(AudioFormat *self) {
    AudioMemory_free(self);
}

void AudioFormat_setSampleRate(AudioFormat *self, double sampleRate) {
    if (self == nullptr) {
        THROW("AudioFormat_setSampleRate: null self");
        return;
    }
    if (!(sampleRate >= AUDIO_RATE_MIN) || sampleRate > AUDIO_RATE_MAX) {
        THROW("AudioFormat_setSampleRate: rate out of range");
        return;
    }
    (*self).sampleRate = sampleRate;
}

void AudioFormat_setChannels(AudioFormat *self, uint32_t channels) {
    if (self == nullptr) {
        THROW("AudioFormat_setChannels: null self");
        return;
    }
    if (channels == 0u || channels > AUDIO_CHANNELS_MAX) {
        THROW("AudioFormat_setChannels: channels out of range");
        return;
    }
    (*self).channels = channels;
}

void AudioFormat_setFramesPerBlock(AudioFormat *self, uint32_t frames) {
    if (self == nullptr) {
        THROW("AudioFormat_setFramesPerBlock: null self");
        return;
    }
    if (frames == 0u || frames > AUDIO_BLOCK_FRAMES_MAX) {
        THROW("AudioFormat_setFramesPerBlock: frames out of range");
        return;
    }
    (*self).framesPerBlock = frames;
}

void AudioFormat_setSampleType(AudioFormat *self, uint32_t sampleType) {
    if (self == nullptr) {
        THROW("AudioFormat_setSampleType: null self");
        return;
    }
    if (sampleType != AUDIO_SAMPLE_F32 && sampleType != AUDIO_SAMPLE_I16 &&
        sampleType != AUDIO_SAMPLE_I32) {
        THROW("AudioFormat_setSampleType: unknown sample type");
        return;
    }
    (*self).sampleType = sampleType;
}

double AudioFormat_getSampleRate(const AudioFormat *self) {
    return self ? (*self).sampleRate : 0.0;
}

uint32_t AudioFormat_getChannels(const AudioFormat *self) {
    return self ? (*self).channels : 0u;
}

uint32_t AudioFormat_getFramesPerBlock(const AudioFormat *self) {
    return self ? (*self).framesPerBlock : 0u;
}

uint32_t AudioFormat_getSampleType(const AudioFormat *self) {
    return self ? (*self).sampleType : AUDIO_SAMPLE_UNKNOWN;
}

bool AudioFormat_isValid(const AudioFormat *self) {
    if (self == nullptr)
        return false;
    return (*self).sampleRate >= AUDIO_RATE_MIN &&
           (*self).sampleRate <= AUDIO_RATE_MAX &&
           (*self).channels >= 1u && (*self).channels <= AUDIO_CHANNELS_MAX &&
           (*self).framesPerBlock >= 1u && (*self).framesPerBlock <= AUDIO_BLOCK_FRAMES_MAX &&
           (*self).sampleType != AUDIO_SAMPLE_UNKNOWN;
}

void AudioFormat_toString(const AudioFormat *self, char *dest, size_t cap, bool *outTruncated) {
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
    int n = snprintf(dest, cap, "AudioFormat(%.1f Hz, %u ch, %u frames, type=%u)",
                     (*self).sampleRate, (*self).channels,
                     (*self).framesPerBlock, (*self).sampleType);
    if (outTruncated && n >= 0 && (size_t) n >= cap)
        *outTruncated = true;
}

void AudioFormat_toStringStruct(const AudioFormat *self, char *dest, size_t cap, bool *outTruncated) {
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
                     "AudioFormat { sampleRate: %.1f, channels: %u, framesPerBlock: %u, sampleType: %u }",
                     (*self).sampleRate, (*self).channels,
                     (*self).framesPerBlock, (*self).sampleType);
    if (outTruncated && n >= 0 && (size_t) n >= cap)
        *outTruncated = true;
}
