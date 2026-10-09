#ifndef AUDIO_AUDIO_FORMAT_H
#define AUDIO_AUDIO_FORMAT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "c23/constructor.h"

// audio/audio_format.h — the value class describing a PCM stream shape:
// sample rate, channel count, frames per render block, and sample type. It is
// the negotiation record an AudioStream and an AudioBuffer are built from.
//
// Constructors (the Construction and arity clause):
//   AudioFormat();                    // defaults — 44100 Hz, stereo f32, 256-frame blocks
//   AudioFormat(48000);               // rate only
//   AudioFormat(48000, AUDIO_CHANNELS_STEREO);
//   #define AudioFormat(...) CONSTRUCTOR_DISPATCH(...)

#define AUDIO_SAMPLE_UNKNOWN 0u
#define AUDIO_SAMPLE_F32     1u   // 32-bit float, interleaved
#define AUDIO_SAMPLE_I16     2u   // 16-bit signed PCM, interleaved
#define AUDIO_SAMPLE_I32     3u   // 32-bit signed PCM, interleaved

#define AUDIO_CHANNELS_MONO   1u
#define AUDIO_CHANNELS_STEREO 2u

// Named defaults (the No Hardcoding Law) — starting points, not final values.
#define AUDIO_RATE_DEFAULT         44100.0
#define AUDIO_CHANNELS_DEFAULT     AUDIO_CHANNELS_STEREO
#define AUDIO_BLOCK_FRAMES_DEFAULT 256u
#define AUDIO_SAMPLE_TYPE_DEFAULT  AUDIO_SAMPLE_F32

#define AUDIO_RATE_MIN 1.0
#define AUDIO_RATE_MAX 768000.0
#define AUDIO_CHANNELS_MAX 64u
#define AUDIO_BLOCK_FRAMES_MAX 65536u

typedef struct AudioFormat AudioFormat;

AudioFormat *AudioFormat_0(void);
AudioFormat *AudioFormat_1(double sampleRate);
AudioFormat *AudioFormat_2(double sampleRate, uint32_t channels);
#define AudioFormat(...) CONSTRUCTOR_DISPATCH(AudioFormat, __VA_ARGS__)

// Release the block back to the Samplerate arena (null-safe).
void AudioFormat_free(AudioFormat *self);

// Setters validate at least as strictly as the getters: an out-of-range value is
// rejected (state preserved) and a cold rejection is reported with THROW. The
// getters are null-safe and return safe zero defaults.
void AudioFormat_setSampleRate(AudioFormat *self, double sampleRate);
void AudioFormat_setChannels(AudioFormat *self, uint32_t channels);
void AudioFormat_setFramesPerBlock(AudioFormat *self, uint32_t frames);
void AudioFormat_setSampleType(AudioFormat *self, uint32_t sampleType);

double AudioFormat_getSampleRate(const AudioFormat *self);
uint32_t AudioFormat_getChannels(const AudioFormat *self);
uint32_t AudioFormat_getFramesPerBlock(const AudioFormat *self);
uint32_t AudioFormat_getSampleType(const AudioFormat *self);

// True when the current field combination describes a usable stream shape.
bool AudioFormat_isValid(const AudioFormat *self);

// Bounded string projections (the toString Law). Cold path only; a null self
// writes "nullptr". toStringStruct mirrors the fields one layer deep.
void AudioFormat_toString(const AudioFormat *self, char *dest, size_t cap, bool *outTruncated);
void AudioFormat_toStringStruct(const AudioFormat *self, char *dest, size_t cap, bool *outTruncated);

#endif // AUDIO_AUDIO_FORMAT_H
