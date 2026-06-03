/*
  Implementation readapted from:
  https://github.com/FedericoDiMarzo/stm32-monosynth

  Credits:
  https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf
  https://www.kvraudio.com/forum/viewtopic.php?t=368466
*/

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/fast_math.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Abstract base class for audio filters.
 *
 * The subclass must implement the following processing method:
 * real_type filter_sample(real_type input) noexcept;
 */
class BaseFilter : public BaseProcessor<BaseFilter>
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    uint32_t sample_rate;
    real_type frequency;
    real_type resonance;

    // Construction // == == == == == == == == == == == == == == == == == == == ==


  public:
    enum class FilterType
    {
        LowPass,
        HighPass,
    };


    AT_DISABLE_COPY_AND_MOVE_M(BaseFilter);

    /**
     * @param sample_rate Sample rate of the audio system in Hz.
     * @param frequency Frequency of the filter in Hz.
     * @param resonance Resonance of the filter (between 0 and 1).
     */
    BaseFilter(uint32_t sample_rate, real_type frequency, real_type resonance)
        : BaseProcessor(1, 1),
          sample_rate(sample_rate),
          frequency(frequency),
          resonance(resonance){};

    virtual ~BaseFilter() = default;

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    virtual inline void set_frequency(real_type frequency) noexcept
    {
        this->frequency = frequency;
    };

    inline real_type get_frequency() const noexcept
    {
        return frequency;
    };

    virtual inline void set_resonance(real_type resonance) noexcept
    {
        this->resonance = std::clamp(resonance, 0.0f, 1.0f);
    }

    inline real_type get_resonance() const noexcept
    {
        return resonance;
    }

    inline void set_sample_rate(uint32_t sample_rate) noexcept
    {
        this->sample_rate = sample_rate;
        reset();
    }

    inline uint32_t get_sample_rate() const noexcept
    {
        return sample_rate;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    virtual void reset() noexcept = 0;

    inline void process_sample(const real_type* input, real_type* output) noexcept
    {
        *output = filter_sample(*input);
    };

    virtual real_type filter_sample(real_type input) noexcept = 0;
};

/**
 * @brief This function can be used to compute the gain powers for a series of filters.
 * These gains are used to compute the output from the filter series.
 *
 * @param gain_powers Vector to store the gain powers (G, G^2, G^3, ...).
 * @param gain_0 Instantaneous gain of the first filter in the series (G).
 */
inline void filter_series_compute_gain_powers(std::vector<real_type>& gain_powers, real_type gain_0) noexcept
{
    gain_powers[0] = gain_0;
    for (uint32_t i = 1; i < gain_powers.size(); i++)
    {
        gain_powers[i] = gain_powers[i - 1] * gain_0;
    }
};

} // namespace audio_toolbox
