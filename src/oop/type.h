#ifndef OOP_TYPE_H
#define OOP_TYPE_H

#include <stdint.h>

#include "type/type.h"

// oop/type.h — samplerate's OWN class registry (the One Type Registry Law).
//
// The shared type ALGEBRA — mask constants, SUGAR_VEX, FORM_*/PROJ_*/ARCH_*,
// Type_make/Type_class/Type_form/Type_project, Type_arch, and the parent-chain
// resolver — is OWNED by Relational Engine in `type/type.h` and included above.
// This file keeps ONLY samplerate's class numbers, numbered 1..N inside the
// PROJ_SAMPLERATE project byte. Bare numbers here name samplerate's own class
// space; cross-project dispatch ships full TYPE_* ids.

// --- AUDIO DATA ---
#define ID_AUDIO_FORMAT   0x0001u
#define ID_AUDIO_BUFFER   0x0002u

// --- STREAM / DEVICE ---
#define ID_AUDIO_STREAM   0x0003u

#define TYPE_AUDIO_FORMAT  (SUGAR_VEX | PROJ_SAMPLERATE | FORM_SINGLETON | ID_AUDIO_FORMAT)
#define TYPE_AUDIO_BUFFER  (SUGAR_VEX | PROJ_SAMPLERATE | FORM_SINGLETON | ID_AUDIO_BUFFER)
#define TYPE_AUDIO_STREAM  (SUGAR_VEX | PROJ_SAMPLERATE | FORM_SINGLETON | ID_AUDIO_STREAM)

#endif // OOP_TYPE_H
