#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/fast_math.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/filters/base_filter.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief One pole low pass IIR filter based on TPT discretization
 * (zero delay feedback design).
 *
 * @note Reference: Chapter 3.10 of "The art of VA filter design"
 * by Vadim Zavalishin.
 */
class VirtualAnalogFilter1P : public BaseFilter
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    // We specify the implementation at runtime to avoid branching when processing
    struct FilterSpecialization
    {
        FilterType type;
        std::function<real_type(real_type)> filter_sample_fn;
        std::function<real_type(void)> get_instantaneuous_gain_fn;
        std::function<real_type(void)> get_instantaneuous_bias_fn;
    };

    FilterSpecialization specialization;

    real_type s; // State
    real_type g; // Integrator gain
    real_type G; // 1 / (1 + g)


    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(VirtualAnalogFilter1P);


    /**
     * @param sample_rate Sample rate of the audio system in Hz.
     * @param frequency Frequency of the filter in Hz.
     */
    VirtualAnalogFilter1P(uint32_t sample_rate, real_type frequency, FilterType type = FilterType::LowPass);
    virtual ~VirtualAnalogFilter1P() = default;

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    real_type get_instantaneuous_gain() const noexcept;
    real_type get_instantaneuous_bias() const noexcept;

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    inline void reset() noexcept override
    {
        s = 0;
    }

    void set_frequency(real_type cutoff_freq) noexcept override;

    real_type filter_sample(real_type input) noexcept override;

    // Specialized implementations == == == == == == == == == == == == == == == ==

  private:
    real_type filter_sample_lpf(real_type input) noexcept;
    real_type filter_sample_hpf(real_type input) noexcept;
    real_type get_instantaneuous_gain_lpf() const noexcept;
    real_type get_instantaneuous_gain_hpf() const noexcept;
    real_type get_instantaneuous_bias_lpf() const noexcept;
    real_type get_instantaneuous_bias_hpf() const noexcept;
};

} // namespace audio_toolbox
