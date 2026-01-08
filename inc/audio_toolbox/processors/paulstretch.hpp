#ifndef _AUDIO_TOOLBOX_PAULSTRETCH_H_
#define _AUDIO_TOOLBOX_PAULSTRETCH_H_
/*
  Adapted from
  https://github.com/paulnasca/paulstretch_python
  Differently from the rest of this codebase, this implementation is not
  licensed under the MIT License. Instead, it is provided under the GNU
  General Public License v3.0.
*/
#include <array>
#include <atomic>
#include <complex>
#include <cstdint>
#include <random>
#include <variant>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/circular_buffer.hpp"
#include "audio_toolbox/core/constexpr.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/stft.hpp"

#define _PAUL_STRETCH_TEMPLATE                \
  template <uint32_t SAMPLE_RATE, uint32_t N, \
            std::array<uint32_t, N> FFT_LEN_ARRAY, uint32_t BUFFER_LEN>

#define _PAUL_STRETCH_TYPE \
  PaulStretch<SAMPLE_RATE, N, FFT_LEN_ARRAY, BUFFER_LEN>

namespace audio_toolbox {

/**
 * @brief Extreme time stretching algorithm.
 *
 * @tparam SAMPLE_RATE Sample rate of the audio.
 * @tparam N Number of FFT sizes.
 * @tparam FFT_LEN_ARRAY Array of FFT sizes.
 * @tparam BUFFER_LEN Total size of the buffer. By default > 30 seconds.
 */
template <uint32_t SAMPLE_RATE, uint32_t N,
          std::array<uint32_t, N> FFT_LEN_ARRAY,
          uint32_t BUFFER_LEN = next_power_of_two(SAMPLE_RATE * 30)>
class PaulStretch : public MonoProcessor<_PAUL_STRETCH_TYPE> {
  static_assert(N > 0, "At least one FFT size must be provided.");

  static constexpr uint32_t MAX_FFT_SIZE = array_max(FFT_LEN_ARRAY);
  static constexpr uint32_t MAX_N_FREQS = MAX_FFT_SIZE / 2 + 1;
  static constexpr uint32_t MAX_OLA_LEN = MAX_FFT_SIZE / 2;

  /**
   * @brief Helper to create a variant of STFT types for each FFT size.
   */
  template <std::size_t... Is>
  static constexpr auto _make_stft_variant_helper(std::index_sequence<Is...>) {
    return std::variant<STFT<FFT_LEN_ARRAY[Is]>...>{};
  }

  /**
   * @brief The type of the STFT variant.
   */
  using STFTVariant =
      decltype(_make_stft_variant_helper(std::make_index_sequence<N>{}));

  /**
   * @brief Static helper to create the array of STFT instances.
   */
  static constexpr auto make_stfts() {
    return []<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::array<STFTVariant, N>{
          STFTVariant{std::in_place_type<STFT<FFT_LEN_ARRAY[Is]>>}...};
    }(std::make_index_sequence<N>{});
  }

  /**
   * @brief Holds the STFT instances for each FFT size.
   */
  std::array<STFTVariant, N> stft_array = make_stfts();

  // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
  // There are two states
  // 1. Rec mode: fill the buffer with the input samples while playing them.
  // 2. Proc mode: process the samples from the buffer.
  std::atomic<bool> is_recording_ = true;
  // Position in the recording buffer to read from
  std::atomic<real_type> seek_pos = 1.0f;
  // Amount of phase randomization [0, 1]
  std::atomic<real_type> phase_rand_amt = 1.0f;
  // Wraps at OLA_LEN
  uint32_t ola_count = 0;
  // Counts the number of samples recorded
  uint32_t rec_count = 0;
  // Working buffer for the FFT
  AudioBuffer<MAX_OLA_LEN> fft_buff;
  // Recording buffer
  CircularBuffer<BUFFER_LEN> rec_buffer;
  // Holds the frequency domain data after the FFT
  AudioBuffer<2 * MAX_N_FREQS> freq_buff;
  // Index of the current FFT size in use
  std::atomic<uint32_t> current_fft_index = 0;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(PaulStretch);

  PaulStretch();
  ~PaulStretch() = default;

  void reset();

  /**
   * @brief Select the FFT size to use for the next analysis.
   *
   * @param index Index of the FFT size in WIN_LEN_A.
   */
  inline void set_fft_index(uint32_t index) noexcept;

  /**
   * @brief Get the current FFT size index.
   */
  inline uint32_t get_fft_index() const noexcept;

  /**
   * @brief Get the current FFT size.
   */
  inline uint32_t get_fft_size() const noexcept;

  /**
   * @brief Switch between recording and processing mode.
   */
  inline void toggle_recording() noexcept;

  /**
   * @brief Set the recording state.
   *
   * @param state true to start recording, false to start processing.
   */
  inline void set_recording(bool state) noexcept;

  /**
   * @brief Get the recording state.
   *
   * @return true if recording, false otherwise.
   */
  inline bool is_recording() const noexcept;

  /**
   * @brief Set the seek position.
   *
   * @param seek Seek position in the range [0, 1].
   */
  inline void set_seek_position(real_type seek) noexcept;

  /**
   * @brief Get the seek position. In the range [0, 1].
   */
  inline real_type get_seek_position() const noexcept;

  /**
   * @brief Set the amount of phase randomization.
   *
   * @param amt Amount of phase randomization in the range [0, 1].
   */
  inline void set_phase_randomization_amount(real_type amt) noexcept;

  /**
   * @brief Get the amount of phase randomization. In the range [0, 1].
   */
  inline real_type get_phase_randomization_amount() const noexcept;

  /**
   * @brief Get the number of recorded samples.
   */
  inline uint32_t get_num_recorded_samples() const noexcept;

  /**
   * @brief Get the current FFT size.
   */
  inline uint32_t get_current_fft_size() const noexcept;

  /**
   * @brief Get the total length of the recording buffer.
   */
  inline uint32_t get_buffer_length() const noexcept { return BUFFER_LEN; }

  inline real_type process_sample(real_type x) noexcept;

 private:
  /**
   * @brief Generate a new frame from the recording buffer.
   */
  void generate_new_frame() noexcept;

  /**
   * @brief Process a step of the algorithm.
   *
   * @return real_type Output sample.
   */
  real_type process_step() noexcept;

  /**
   * @brief Get the length of the overlap-add buffer.
   */
  inline uint32_t get_ola_len() const noexcept;

  /**
   * @brief Get the number of frequency bins for the current FFT size.
   */
  inline uint32_t get_n_freqs() const noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/paulstretch_imp.hpp"

#endif  // _AUDIO_TOOLBOX_PAULSTRETCH_H_
