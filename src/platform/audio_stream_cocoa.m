#import <AudioToolbox/AudioToolbox.h>
#import <CoreAudio/CoreAudio.h>

#include <stdio.h>
#include <string.h>

#include "stream/audio_stream.h"

#include "core/audio_memory.h"
#include "oop/type.h"

#include "annotation/definition.h"
#include "annotation/overview.h"
#include "annotation/what.h"
#include "exception/throw.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: AudioStream (platform/audio_stream_cocoa.m)
 * ============================================================================
 * The macOS implementation of stream/audio_stream.h: a default-output AudioUnit
 * HAL with a float PCM stream format and a render callback that feeds the
 * caller's realtime callback straight into hardware output at minimal buffer
 * latency. This is the audio counterpart to graphvex's presentation backend —
 * the same contract on every platform, a native unit behind it.
 *
 * The AudioStream block is carved from the Samplerate arena; the AudioUnit is
 * the OS-owned resource and is disposed in AudioStream_free. The render callback
 * runs on the audio thread and never allocates, locks, or waits: it calls the
 * registered callback or zero-fills (the Zero-Allocation Audio Callback Law).
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: AudioStream (platform/audio_stream_cocoa.m)
 * ============================================================================
 * macOS CoreAudio default-output AudioUnit HAL for realtime PCM output.
 *
 * STRUCT FIELDS:
 * ----------------------------------------------------------------------------
 *   AudioComponentInstance unit;        // default-output AudioUnit (OS-owned)
 *   bool   running;                     // AudioOutputUnitStart succeeded
 *   double sampleRate;                  // negotiated Hz
 *   uint32_t channels;                  // negotiated channel count
 *   uint32_t framesPerBlock;            // requested render quantum (frames)
 *   AudioRenderCallback callback;       // caller's realtime pull
 *   ;;WHAT("caller render context")
 *   void *userData;                     // passed back to callback
 *
 * PUBLIC SURFACE (stream/audio_stream.h):
 *   Constructors: AudioStream_0(), AudioStream_1(config)
 *   Lifecycle: AudioStream_open/_start/_stop/_free/_isRunning
 *   Metrics:   AudioStream_getSampleRate/_getChannels/_getFramesPerBlock/_getLatencyMs
 *   Strings:   AudioStream_toString(), AudioStream_toStringStruct()
 * ============================================================================
 */

#define AUDIO_STREAM_DEFAULT_RATE_HZ  48000.0
#define AUDIO_STREAM_DEFAULT_CHANNELS 2u

typedef struct AudioStream {
    AudioComponentInstance unit;
    bool running;
    double sampleRate;
    uint32_t channels;
    uint32_t framesPerBlock;
    AudioRenderCallback callback;
    ;;WHAT("caller render context")
    void *userData;
} AudioStream;

static OSStatus audioStreamRender(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
    (void) ioActionFlags;
    (void) inTimeStamp;
    (void) inBusNumber;

    AudioStream *self = (AudioStream*) inRefCon;
    if (self == nullptr || ioData == nullptr || (*ioData).mNumberBuffers == 0u)
        return noErr;
    float *out = (float*) (*ioData).mBuffers[0].mData;
    if (out == nullptr)
        return noErr;
    uint32_t channels = (*self).channels;
    if ((*self).callback != nullptr)
        (*self).callback(out, (uint32_t) inNumberFrames, channels, (*self).userData);
    else
        memset(out, 0, (size_t) inNumberFrames * (size_t) channels * sizeof(float));
    return noErr;
}

AudioStream *AudioStream_1(const AudioStreamConfig *config) {
    AudioStream *self = (AudioStream*) AudioMemory_alloc(TYPE_AUDIO_STREAM, sizeof(AudioStream));
    if (self == nullptr)
        return nullptr;
    (*self).unit = nullptr;
    (*self).running = false;
    (*self).sampleRate = (config && (*config).sampleRate > 0.0)
        ? (*config).sampleRate : AUDIO_STREAM_DEFAULT_RATE_HZ;
    (*self).channels = (config && (*config).channels > 0u)
        ? (*config).channels : AUDIO_STREAM_DEFAULT_CHANNELS;
    (*self).framesPerBlock = (config && (*config).framesPerBlock > 0u)
        ? (*config).framesPerBlock : 0u;
    (*self).callback = config ? (*config).callback : nullptr;
    (*self).userData = config ? (*config).userData : nullptr;
    return self;
}

AudioStream *AudioStream_0(void) {
    return AudioStream_1(nullptr);
}

bool AudioStream_open(AudioStream *self) {
    if (self == nullptr) {
        THROW("AudioStream_open: null self");
        return false;
    }
    if ((*self).unit != nullptr)
        return true;

    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    AudioComponent comp = AudioComponentFindNext(nullptr, &desc);
    if (comp == nullptr) {
        THROW("AudioStream_open: no default output unit");
        return false;
    }

    AudioComponentInstance au = nullptr;
    OSStatus err = AudioComponentInstanceNew(comp, &au);
    if (err != noErr || au == nullptr) {
        THROW("AudioStream_open: instance creation failed");
        return false;
    }

    AudioStreamBasicDescription asbd;
    memset(&asbd, 0, sizeof(asbd));
    asbd.mSampleRate = (*self).sampleRate;
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    asbd.mBytesPerPacket = (*self).channels * sizeof(float);
    asbd.mFramesPerPacket = 1;
    asbd.mBytesPerFrame = (*self).channels * sizeof(float);
    asbd.mChannelsPerFrame = (*self).channels;
    asbd.mBitsPerChannel = 32;

    err = AudioUnitSetProperty(au, kAudioUnitProperty_StreamFormat,
                               kAudioUnitScope_Input, 0, &asbd, sizeof(asbd));
    if (err != noErr) {
        AudioComponentInstanceDispose(au);
        THROW("AudioStream_open: stream format rejected");
        return false;
    }

    AURenderCallbackStruct cb;
    cb.inputProc = audioStreamRender;
    cb.inputProcRefCon = (void*) self;
    err = AudioUnitSetProperty(au, kAudioUnitProperty_SetRenderCallback,
                               kAudioUnitScope_Input, 0, &cb, sizeof(cb));
    if (err != noErr) {
        AudioComponentInstanceDispose(au);
        THROW("AudioStream_open: render callback rejected");
        return false;
    }

    err = AudioUnitInitialize(au);
    if (err != noErr) {
        AudioComponentInstanceDispose(au);
        THROW("AudioStream_open: unit initialize failed");
        return false;
    }

    (*self).unit = au;
    return true;
}

bool AudioStream_start(AudioStream *self) {
    if (self == nullptr || (*self).unit == nullptr) {
        THROW("AudioStream_start: stream not open");
        return false;
    }
    if ((*self).running)
        return true;
    OSStatus err = AudioOutputUnitStart((*self).unit);
    if (err != noErr) {
        THROW("AudioStream_start: output start failed");
        return false;
    }
    (*self).running = true;
    return true;
}

bool AudioStream_stop(AudioStream *self) {
    if (self == nullptr || (*self).unit == nullptr)
        return false;
    if (!(*self).running)
        return true;
    OSStatus err = AudioOutputUnitStop((*self).unit);
    if (err != noErr) {
        THROW("AudioStream_stop: output stop failed");
        return false;
    }
    (*self).running = false;
    return true;
}

bool AudioStream_isRunning(const AudioStream *self) {
    return self ? (*self).running : false;
}

void AudioStream_free(AudioStream *self) {
    if (self == nullptr)
        return;
    if ((*self).running)
        AudioStream_stop(self);
    if ((*self).unit != nullptr) {
        AudioUnitUninitialize((*self).unit);
        AudioComponentInstanceDispose((*self).unit);
        (*self).unit = nullptr;
    }
    AudioMemory_free(self);
}

double AudioStream_getSampleRate(const AudioStream *self) {
    return self ? (*self).sampleRate : 0.0;
}

uint32_t AudioStream_getChannels(const AudioStream *self) {
    return self ? (*self).channels : 0u;
}

uint32_t AudioStream_getFramesPerBlock(const AudioStream *self) {
    return self ? (*self).framesPerBlock : 0u;
}

double AudioStream_getLatencyMs(const AudioStream *self) {
    if (self == nullptr || (*self).sampleRate <= 0.0 || (*self).framesPerBlock == 0u)
        return 0.0;
    return (1000.0 * (double) (*self).framesPerBlock) / (*self).sampleRate;
}

void AudioStream_toString(const AudioStream *self, char *dest, size_t cap, bool *outTruncated) {
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
    int n = snprintf(dest, cap, "AudioStream(%s, %.1f Hz, %u ch, %.2f ms)",
                     (*self).running ? "running" : "stopped",
                     (*self).sampleRate, (*self).channels,
                     AudioStream_getLatencyMs(self));
    if (outTruncated && n >= 0 && (size_t) n >= cap)
        *outTruncated = true;
}

void AudioStream_toStringStruct(const AudioStream *self, char *dest, size_t cap, bool *outTruncated) {
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
                     "AudioStream { unit: %s, running: %s, sampleRate: %.1f, channels: %u, framesPerBlock: %u, callback: %s }",
                     (*self).unit ? "open" : "closed",
                     (*self).running ? "true" : "false",
                     (*self).sampleRate, (*self).channels, (*self).framesPerBlock,
                     (*self).callback ? "set" : "none");
    if (outTruncated && n >= 0 && (size_t) n >= cap)
        *outTruncated = true;
}
