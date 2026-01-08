#ifndef _AUDIO_TOOLBOX_FFT_H_
#define _AUDIO_TOOLBOX_FFT_H_

#include <array>
#include <complex>
#include <cstdint>
#include <cstring>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"

#if AT_FFT_IMPLEMENTATION == 0
#include <pffft/pffft.h>
#elif AT_FFT_IMPLEMENTATION == 1
#include <arm/cmsis_fft/arm_math.h>
#else
#error \
    "No FFT implementation selected set -DAT_FFT_IMPLEMENTATION=<pffft=0|arm_cmsis=1>"
#endif

#define _FFT_TYPE FFT<FFT_SIZE>
#define _FFT_TEMPLATE template <uint32_t FFT_SIZE>

namespace audio_toolbox {

/**
 * @brief The type of analysis/synthesis window to use for the FFT.
 *
 * @var HanningSqrt The square root of the Hann window.
 */
enum class FFTWindowType {
  HanningSqrt,
};

/**
 * @brief Performs a Fast Fourier transform (FFT) on a signal and
 * its inverse (IFFT).
 */
template <uint32_t FFT_SIZE>
class FFT {
  static constexpr uint32_t N_FREQS = FFT_SIZE / 2 + 1;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(FFT);

  FFT();

  ~FFT();

  /**
   * @brief Compute the FFT of the input signal.
   *
   * @note Input and output can alias.
   * @param input Input signal (FFT_SIZE).
   * @param output Output signal (N_FREQS, 2).
   */
  void compute_fft(const real_type* input, real_type* output) noexcept;

  /**
   * @brief Compute the IFFT of the input signal.
   *
   * @note Input and output can alias.
   * @param input Input signal (N_FREQS, 2).
   * @param output Output signal (FFT_SIZE).
   */
  void compute_ifft(const real_type* input, real_type* output) noexcept;

 private:
#if AT_FFT_IMPLEMENTATION == 0
  PFFFT_Setup* pffft_setup;
  AudioBuffer<2 * N_FREQS> work_buffer;
  AudioBuffer<2 * N_FREQS> process_buffer;
#elif AT_FFT_IMPLEMENTATION == 1
  arm_rfft_fast_instance_f32 arm_rfft_instance;
  AudioBuffer<2 * N_FREQS> process_buffer;
#endif
};

}  // namespace audio_toolbox

#if AT_FFT_IMPLEMENTATION == 0
#include "audio_toolbox/processors/fft_pffft_imp.hpp"
#elif AT_FFT_IMPLEMENTATION == 1
#include "audio_toolbox/processors/fft_arm_cmsis_fft_imp.hpp"
#endif

#endif  // _AUDIO_TOOLBOX_FFT_H_