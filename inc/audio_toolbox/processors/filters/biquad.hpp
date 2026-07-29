#pragma once

/*
  Biquadratic filter.

  Adapted from https://raw.githubusercontent.com/tomlankhorst/biquad
  Specific filter designs from https://www.w3.org/TR/audio-eq-cookbook
 */

#include <complex>
#include <cstdint>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Filter types.
 *
 * @var lowpass Lowpass filter.
 * @var highpass Highpass filter.
 * @var lowshelf Lowshelf filter.
 * @var highshelf Highshelf filter.
 * @var bandpass Bandpass filter.
 * @var notch Notch filter.
 * @var allpass Allpass filter.
 */
enum class BiquadFilterType
{
    Lowpass,
    Highpass,
    Lowshelf,
    Highshelf,
    Bandpass,
    Notch,
    Allpass,
    Peak,
};

/**
 * @brief Biquadratic filter.
 */
class Biquad : public BaseProcessor<Biquad>
{
  public:
    AT_DISABLE_COPY_AND_MOVE_M(Biquad);

    /**
     * @brief Initialize a normalized biquad filter.
     *
     * @param b0
     * @param b1
     * @param b2
     * @param a1
     * @param a2
     * @return Biquad instance.
     */
    Biquad(real_type b0, real_type b1, real_type b2, real_type a1, real_type a2);

    /**
     * @brief Initialize a biquad filter with all six coefficients.
     *
     * @param b0
     * @param b1
     * @param b2
     * @param a0
     * @param a1
     * @param a2
     * @return Biquad instance.
     */
    Biquad(real_type b0, real_type b1, real_type b2, real_type a0, real_type a1, real_type a2);

    /**
     * @brief Initialize a biquad filter with a given type.
     *
     * @param type Filter type.
     * @param sample_rate Sample rate of the filter.
     * @param frequency Center frequency of the filter.
     * @param q Quality factor of the filter.
     * @param gain_db Gain of the filter in decibels.
     */
    Biquad(BiquadFilterType type, uint32_t sample_rate, real_type frequency, real_type q, real_type gain_db = 0.0f);

    void process_sample(const real_type* input, real_type* output) noexcept;

    /**
     * @brief Return poles of the Biquad filter.
     *
     * @return vector of std::complex poles.
     */
    std::vector<std::complex<real_type>> poles();

    /**
     * Return zeros of the Biquad filter.
     * @return vector of std::complex zeros.
     */
    std::vector<std::complex<real_type>> zeros();

    /**
     * @brief Checks if all poles lie within the unit-circle.
     *
     * @return boolean whether the filter is stable or not.
     */
    bool is_stable();

  private:
    real_type B[3];
    real_type A[2];
    real_type wz[2];

    /**
     * @brief Sets the gain parameters.
     *
     * @param b0
     * @param b1
     * @param b2
     * @param a1
     * @param a2
     */
    void set(real_type b0, real_type b1, real_type b2, real_type a1, real_type a2);

    /**
     * @brief Sets the gain parameters for a lowpass filter.
     */
    void design_lowpass(real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for a highpass filter.
     */
    void design_highpass(real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for a lowshelf filter.
     */
    void design_lowshelf(real_type A, real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for a highshelf filter.
     */
    void design_highshelf(real_type A, real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for a bandpass filter.
     */
    void design_bandpass(real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for a notch filter.
     */
    void design_notch(real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for a peak filter.
     */
    void design_peak(real_type A, real_type alpha, real_type cs);

    /**
     * @brief Sets the gain parameters for an allpass filter.
     */
    void design_allpass(real_type alpha, real_type cs);
};

} // namespace audio_toolbox
