#pragma once

#include <cstdint>
#include <vector>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/spectral/fft.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Performs a short-time Fourier transform (STFT) on a signal and
 * its inverse.
 */
class STFT
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==
    FFT fft;
    AudioBuffer window;
    AudioBuffer process_buffer;
    AudioBuffer stft_buffer;
    AudioBuffer ola_buffer;

    // Construction // == == == == == == == == == == == == == == == == == == == ==
  public:
    AT_DISABLE_COPY_AND_MOVE_M(STFT);

    /**
     * @brief The type of analysis/synthesis window to use for the STFT.
     *
     * @var HanningSqrt The square root of the Hann window.
     */
    enum class WindowType
    {
        HanningSqrt,
    };

    /**
     * @param win_size Size of the analysis/synthesis window. Must be less than or
     * equal to fft_size
     * @param fft_size Number of points to use for the FFT. Must be a power of
     * two.
     * @param window_type Select the type of window to use for the STFT. Default
     * is HanningSqrt.
     */
    STFT(uint32_t win_size, uint32_t fft_size, WindowType window_type = WindowType::HanningSqrt);
    ~STFT(){};

    // Getters // == == == == == == == == == == == == == == == == == == == == ==

    inline uint32_t get_win_size() const noexcept
    {
        return window.get_num_samples();
    }

    inline uint32_t get_fft_size() const noexcept
    {
        return fft.get_fft_size();
    }

    inline uint32_t get_num_freqs() const noexcept
    {
        return fft.get_n_freqs();
    }

    // TODO: Support different hop sizes
    inline uint32_t get_hopsize() const noexcept
    {
        return window.get_num_samples() / 2;
    }

    inline const real_type* get_window_write_ptr() const noexcept
    {
        return window.get_read_ptr();
    }

    inline FFT& get_fft() noexcept
    {
        return fft;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    void reset() noexcept;

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

  private:
    /**
     * @brief Initialize the STFT window.
     */
    void init_window(WindowType window_type);
};

} // namespace audio_toolbox
