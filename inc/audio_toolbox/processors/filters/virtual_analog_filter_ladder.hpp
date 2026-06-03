#pragma once


#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/fast_math.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/filters/base_filter.hpp"
#include "audio_toolbox/processors/filters/virtual_analog_filter_one_pole.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{


/**
 * @brief Two poles ladder low pass IIR filter based on TPT discretization.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
class VirtualAnalogFilterLadder : public BaseFilter
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    static constexpr real_type MAX_RESONANCE = 3.9f;

    std::vector<std::unique_ptr<VirtualAnalogFilter1P>> one_pole_filters;
    std::vector<real_type> gain_powers;
    uint32_t num_poles;


    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(VirtualAnalogFilterLadder);


    /**
     * @param sample_rate Sample rate of the audio system in Hz.
     * @param frequency Frequency of the filter in Hz.
     * @param resonance Resonance of the filter (between 0 and 1).
     * @param num_poles Number of poles of the filter (>=2).
     * @param filter_type Type of the filter {LowPass, HighPass}.
     */
    VirtualAnalogFilterLadder(uint32_t sample_rate,
                              real_type frequency,
                              real_type resonance,
                              uint32_t num_poles = 4,
                              FilterType filter_type = FilterType::LowPass);
    virtual ~VirtualAnalogFilterLadder() = default;

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==


    // Processing // == == == == == == == == == == == == == == == == == == == ==

    inline void reset() noexcept override
    {
        for (auto& filter : one_pole_filters)
        {
            filter->reset();
        }
    }

    void set_frequency(real_type cutoff_freq) noexcept override;

    real_type filter_sample(real_type input) noexcept override;
};

} // namespace audio_toolbox
