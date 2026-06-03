#pragma once

#include <array>
#include <complex>
#include <cstdint>
#include <cstring>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/constexpr.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

#if AT_FFT_IMPLEMENTATION == 0
    #include <pffft/pffft.h>
#elif AT_FFT_IMPLEMENTATION == 1
    #include <arm/cmsis_fft/arm_math.h>
#else
    #error "No FFT implementation selected set -DAT_FFT_IMPLEMENTATION=<pffft=0|arm_cmsis=1>"
#endif

namespace audio_toolbox
{

/**
 * @brief Performs a Fast Fourier transform (FFT) on a signal and
 * its inverse (IFFT).
 */
class FFT
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    uint32_t fft_size;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(FFT);

    FFT(uint32_t fft_size);
    ~FFT();

    // Getters // == == == == == == == == == == == == == == == == == == == == ==

    inline uint32_t get_fft_size() const noexcept
    {
        return fft_size;
    }

    inline uint32_t get_n_freqs() const noexcept
    {
        return (fft_size / 2) + 1;
    }

    /**
     * @brief Compute the FFT of the input signal.
     *
     * @note Input and output can alias.
     * @param input Input signal (FFT_SIZE).
     * @param output Output signal (N_FREQS, 2).
     */
    void process(const real_type* input, real_type* output) noexcept;

    /**
     * @brief Compute the IFFT of the input signal.
     *
     * @note Input and output can alias.
     * @param input Input signal (N_FREQS, 2).
     * @param output Output signal (FFT_SIZE).
     */
    void process_inverse(const real_type* input, real_type* output) noexcept;

    // Type-dependent attributes // == == == == == == == == == == == == == == ==

  private:
#if AT_FFT_IMPLEMENTATION == 0
    PFFFT_Setup* pffft_setup;
    AudioBuffer work_buffer{ 2 * get_n_freqs() };
    AudioBuffer process_buffer{ 2 * get_n_freqs() };
#elif AT_FFT_IMPLEMENTATION == 1
    arm_rfft_fast_instance_f32 arm_rfft_instance;
    AudioBuffer process_buffer{ 2 * get_n_freqs() };
#endif
    // == == == == == == == == == == == == == == == == == == == == == == == == ==
};

} // namespace audio_toolbox
