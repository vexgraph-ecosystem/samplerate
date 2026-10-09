#ifndef AUDIO_AUDIO_BUFFER_H
#define AUDIO_AUDIO_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "c23/constructor.h"

// audio/audio_buffer.h — a flat, interleaved 32-bit-float PCM block owned by
// the Samplerate arena. Frames, capacity, channels and sample rate are explicit
// and the backing store is one contiguous arena block, so a consumer sweeps it
// linearly and never pointer-chases (the Data-Oriented Storage Law).
//
// The buffer is prepared cold. The realtime callback only reads the borrowed
// pointer from AudioBuffer_getConstData; it never resizes or allocates (the
// Zero-Allocation Audio Callback Law).
//
// Constructors:
//   AudioBuffer();                              // default block capacity
//   AudioBuffer(512);                           // frames = capacity
//   AudioBuffer(512, AUDIO_CHANNELS_STEREO);
//   AudioBuffer(512, AUDIO_CHANNELS_STEREO, 48000.0);

#define AUDIO_BUFFER_FRAMES_MAX   (1u << 24)   // 16,777,216 frames per block

typedef struct AudioBuffer AudioBuffer;

AudioBuffer *AudioBuffer_0(void);
AudioBuffer *AudioBuffer_1(uint32_t frames);
AudioBuffer *AudioBuffer_2(uint32_t frames, uint32_t channels);
AudioBuffer *AudioBuffer_3(uint32_t frames, uint32_t channels, double sampleRate);
#define AudioBuffer(...) CONSTRUCTOR_DISPATCH(AudioBuffer, __VA_ARGS__)

// Release the block (and its backing PCM) to the Samplerate arena. Null-safe.
void AudioBuffer_free(AudioBuffer *self);

// Set the valid frame count, growing the backing store when needed. Cold path
// only. Returns false (state preserved) on a null self, an oversized request, or
// allocation failure.
bool AudioBuffer_resize(AudioBuffer *self, uint32_t frames);

// Zero the valid frames (metadata untouched).
void AudioBuffer_clear(AudioBuffer *self);

// Read-only accessors. The borrowed pointer is valid until resize/free.
const float *AudioBuffer_getConstData(const AudioBuffer *self);
float *AudioBuffer_getData(AudioBuffer *self);

uint32_t AudioBuffer_getFrames(const AudioBuffer *self);
uint32_t AudioBuffer_getCapacity(const AudioBuffer *self);
uint32_t AudioBuffer_getChannels(const AudioBuffer *self);
double AudioBuffer_getSampleRate(const AudioBuffer *self);

// Validated setters (reject-or-preserve with a cold THROW).
void AudioBuffer_setChannels(AudioBuffer *self, uint32_t channels);
void AudioBuffer_setSampleRate(AudioBuffer *self, double sampleRate);

bool AudioBuffer_isValid(const AudioBuffer *self);

// Bounded string projections (the toString Law). Cold path only.
void AudioBuffer_toString(const AudioBuffer *self, char *dest, size_t cap, bool *outTruncated);
void AudioBuffer_toStringStruct(const AudioBuffer *self, char *dest, size_t cap, bool *outTruncated);

#endif // AUDIO_AUDIO_BUFFER_H
