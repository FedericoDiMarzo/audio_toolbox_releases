#pragma once

/*
  Adapted from
  https://github.com/paulnasca/paulstretch_python
  Differently from the rest of this codebase, this implementation is not
  licensed under the MIT License. Instead, it is provided under the GNU
  General Public License v3.0.
*/
#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/circular_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/spectral/stft.hpp"
#include "audio_toolbox/static/constexpr.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

// TODO: Broken after refactoring

namespace audio_toolbox
{

/**
 * @brief Extreme time stretching algorithm.
 *
 * Allows to select different FFT sizes and to scan the spectral content through
 * a recording buffer. The phase of the scanned section is randomized to produce
 * the characteristic "smearing" effect of the algorithm. The amount of phase
 * randomization can be controlled.
 *
 * Time stretching is achieved seeking through the recording buffer at a
 * different speeds.
 *
 * The processor operates in two modes:
 * 1. Rec mode: fill the buffer with the input samples while playing them.
 * 2. Proc mode: process the samples from the buffer.
 */
class PaulStretch : public BaseProcessor<PaulStretch>
{
    using STFTVector = std::vector<std::unique_ptr<STFT>>;

    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    uint32_t buffer_len;

    // Recording buffer
    CircularBuffer<real_type> rec_buffer;
    // Working buffer for the FFT
    AudioBuffer fft_buff;
    // Holds the frequency domain data after the FFT
    AudioBuffer freq_buff;

    STFTVector stfts;

    // Determines the current mode of the processor: recording or processing.
    std::atomic<bool> is_recording_ = true;
    // Position in the recording buffer to read from
    std::atomic<real_type> seek_pos = 1.0f;
    // Amount of phase randomization [0, 1]
    std::atomic<real_type> phase_rand_amt = 1.0f;
    // Index of the current FFT size in use
    std::atomic<uint32_t> current_fft_index = 0;

    // Wraps at OLA Len
    uint32_t ola_count = 0;
    // Counts the number of samples recorded
    uint32_t rec_count = 0;

    // Construction // == == == == == == == == == == == == == == == == == == ==
  public:
    AT_DISABLE_COPY_AND_MOVE_M(PaulStretch);

    /**
     * @param sample_rate Sample rate of the audio.
     * @param fft_lengths Array of FFT sizes.
     * @param buffer_len_s Total size of the buffer in seconds. The effective buffer size
     * will be the next power of two of the given size in samples.
     */
    PaulStretch(uint32_t sample_rate, std::initializer_list<uint32_t> fft_lengths, real_type buffer_len_s = 30.0f);
    ~PaulStretch() = default;

    // Getters/Setters // == == == == == == == == == == == == == == == == == ==

    // FFT
    inline uint32_t get_num_ffts() const noexcept
    {
        return stfts.size();
    }

    inline STFT& get_stft() noexcept
    {
        return *stfts[get_fft_index()];
    }

    inline uint32_t get_hopsize() noexcept
    {
        return get_stft().get_hopsize();
    }

    // They should be the same for all the STFTs
    inline uint32_t get_fft_size() noexcept
    {
        return get_stft().get_fft_size();
    }

    inline uint32_t get_win_size() noexcept
    {
        return get_stft().get_win_size();
    }

    inline uint32_t get_num_freqs() noexcept
    {
        return get_stft().get_num_freqs();
    }

    void set_fft_index(uint32_t index) noexcept
    {
        current_fft_index.store(std::min(index, get_num_ffts() - 1));
    }

    inline uint32_t get_fft_index() const noexcept
    {
        return current_fft_index.load();
    }

    // Phase
    inline void set_phase_randomization_amount(real_type amt) noexcept
    {
        phase_rand_amt.store(std::clamp(amt, 0.0f, 1.0f));
    }

    inline real_type get_phase_randomization_amount() const noexcept
    {
        return phase_rand_amt.load();
    }

    // State
    inline void toggle_recording() noexcept
    {
        is_recording_.store(!is_recording());
    }

    inline void set_recording(bool state) noexcept
    {
        is_recording_.store(state);
    }

    inline bool is_recording() const noexcept
    {
        return is_recording_.load();
    }

    // Playback

    inline uint32_t get_buffer_length() const noexcept
    {
        return buffer_len;
    }

    inline void set_seek_position(real_type seek) noexcept
    {
        seek_pos.store(std::clamp(seek, 0.0f, 1.0f));
    }

    inline real_type get_seek_position() const noexcept
    {
        return seek_pos.load();
    }

    inline uint32_t get_num_recorded_samples() const noexcept
    {
        return rec_count;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    void process_sample(const real_type* input, real_type* output) noexcept;
    void reset();

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

    // Getters // == == == == == == == == == == == == == == == == == == == ==

    uint32_t get_max_fft_size() const noexcept;
};

} // namespace audio_toolbox
