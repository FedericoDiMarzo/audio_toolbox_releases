#ifndef _AUDIO_TOOLBOX_STFT_H_
#define _AUDIO_TOOLBOX_STFT_H_

#include <array>
#include <complex>
#include <cstdint>
#include <cstring>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/fft.hpp"

#define _STFT_TYPE STFT<FFT_SIZE, WIN_SIZE, WIN_TYPE, OVERLAP_AMT>

#define _STFT_TEMPLATE                                                     \
  template <uint32_t FFT_SIZE, uint32_t WIN_SIZE, STFTWindowType WIN_TYPE, \
            STFTOverlapAmt OVERLAP_AMT>

namespace audio_toolbox {

/**
 * @brief The type of analysis/synthesis window to use for the STFT.
 *
 * @var HanningSqrt The square root of the Hann window.
 */
enum class STFTWindowType {
  HanningSqrt,
};

/**
 * @brief Overlap amount for the STFT.
 *
 * @var Half Half of the window size.
 */
enum class STFTOverlapAmt {
  Half,
};

/**
 * @brief Performs a short-time Fourier transform (STFT) on a signal and
 * its inverse.
 */
template <uint32_t FFT_SIZE, uint32_t WIN_SIZE = FFT_SIZE,
          STFTWindowType WIN_TYPE = STFTWindowType::HanningSqrt,
          STFTOverlapAmt OVERLAP_AMT = STFTOverlapAmt::Half>
class STFT {
  static const uint32_t N_FREQS = FFT_SIZE / 2 + 1;
  // We support only half overlap for now
  static const uint32_t HOPSIZE = WIN_SIZE / 2;
  std::array<real_type, WIN_SIZE> window;
  AudioBuffer<2 * N_FREQS> process_buffer;
  AudioBuffer<WIN_SIZE> stft_buffer;
  AudioBuffer<HOPSIZE> ola_buffer;
  FFT<FFT_SIZE> fft;

  static_assert(WIN_SIZE <= FFT_SIZE,
                "Window size must be less than or equal to FFT size.");
  static_assert(WIN_SIZE % 2 == 0, "Window size must be even.");

 public:
  AT_DISABLE_COPY_AND_MOVE_M(STFT);

  STFT();

  ~STFT();

  void reset() noexcept;

  inline const real_type* get_window_ptr() const noexcept;

  inline const std::array<real_type, WIN_SIZE>& get_window() const noexcept;

  /**
   * @brief Apply the STFT to the input signal.
   *
   * @param input Input signal (WIN_SIZE/2).
   * @param output Output signal (N_FREQS, 2).
   */
  void process(const real_type* input, real_type* output) noexcept;

  /**
   * @brief Apply the inverse STFT to the input signal.
   *
   * @note The ISTFT adds WIN_SIZE/2 of latency.
   * @param input Input signal (N_FREQS, 2).
   * @param output Output signal (WIN_SIZE/2).
   */
  void process_inverse(const real_type* input, real_type* output) noexcept;

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
  /**
   * @brief Initialize the STFT window.
   */
  void init_window();
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/stft_imp.hpp"

#endif  // _AUDIO_TOOLBOX_STFT_H_