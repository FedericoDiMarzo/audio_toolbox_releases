#ifndef _AUDIO_TOOLBOX_MINIAUDIO_INSTANCE_H_
#define _AUDIO_TOOLBOX_MINIAUDIO_INSTANCE_H_

/*
  That's a wrapper to avoid to use Miniaudio as a header-only library.

  Setting up Miniaudio build options.
  https://miniaud.io/docs/manual
*/

// Disables all backends by default and requires MA_ENABLE_* to enable specific
// backends.
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
// Enables the ALSA backend.
#define MA_ENABLE_ALSA
// Disables the built-in FLAC decoder.
#define MA_NO_FLAC
// Disables the built-in MP3 decoder.
#define MA_NO_MP3
// Disables the node graph and engine APIs.
#define MA_NO_NODE_GRAPH
// Disables generation APIs such a ma_waveform and ma_noise.
#define MA_NO_GENERATION
// Enable printf() output of debug logs (MA_LOG_LEVEL_DEBUG).
#if RA_ENABLE_DEBUGGING == 1
#define MA_DEBUG_OUTPUT
#endif

// Remove warnings in GCC/Clang
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
// Remove warnings in GCC only
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#include <miniaudio/miniaudio.h>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif  // _AUDIO_TOOLBOX_MINIAUDIO_INSTANCE_H_