#pragma once

#include <memory>
#include <span>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/delay/fractional_delay.hpp"
#include "audio_toolbox/processors/oscillators/wavetable_oscillator.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Simple mono chorus effect with multiple modulated delay lines.
 */
class BasicChorus : public BaseProcessor<BasicChorus>
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    real_type mix;
    real_type depth;
    std::vector<std::unique_ptr<FractionalDelay>> delay_lines;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_M(BasicChorus);

    /**
     * @param delay_lines A list of delay lines. The delay range for each delay line is set to [min_delay, max_delay].
     * @param depth Modulation depth. This controls how much the delay is modulated by the LFOs.
     * @param mix Mix ratio in [0, 1].
     */
    BasicChorus(std::span<std::unique_ptr<FractionalDelay>> delay_lines, real_type depth = 0.1f, real_type mix = 1.0f);
    BasicChorus(BasicChorus&&) = default;
    BasicChorus& operator=(BasicChorus&&) = default;


    enum class Preset
    {
        SlowLFOModulator, // Slow LFO modulation (0.1 - 0.5 Hz) for a subtle chorus effect.
    };


    /**
     * @brief Factory method to simplify the creation of a BasicChorus with presets for the
     * delay lines.
     *
     * @param sample_rate Sample rate in Hz, used to calculate delay line buffer sizes and LFO frequencies.
     * @param preset Preset configuration for the delay lines. See Preset enum for options.
     * @param num_delay_lines Number of delay lines to create. Each delay line will have the same configuration based on
     * the preset.
     * @param lut_length Length of the lookup table for the LFOs.
     * @param depth Modulation depth. This controls how much the delay is modulated by the LFOs.
     * @param min_delay Minimum delay in fractional samples for the randomized delay range of each delay line.
     * @param max_delay Maximum delay in fractional samples for the randomized delay range of each delay line.
     * @param mix Mix ratio in [0, 1].
     * @return BasicChorus instance.
     */
    static BasicChorus create(uint32_t sample_rate,
                              Preset preset = Preset::SlowLFOModulator,
                              uint32_t num_delay_lines = 3,
                              uint32_t lut_length = 1024,
                              real_type depth = 0.1f,
                              real_type min_delay = 0.0f,
                              real_type max_delay = 300,
                              real_type mix = 1.0f) noexcept;

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    inline real_type get_depth() const noexcept
    {
        return depth;
    }

    inline void set_depth(real_type depth) noexcept
    {
        this->depth = depth;
        for (auto& dl : delay_lines)
        {
            dl->set_modulation_depth(depth * dl->get_modulation_depth());
        }
    }

    inline real_type get_mix() const noexcept
    {
        return mix;
    }

    inline void set_mix(real_type mix) noexcept
    {
        this->mix = clip(mix, 0.0f, 1.0f);
    }

    /**
     * @param freq Target modulation frequency in Hz.
     * @param freq_max If provided and higher than freq, the LFOs will be
     * modulated between freq and freq_max.
     */
    void set_modulation_frequency(real_type freq, real_type freq_max = -1.0f) noexcept
    {
        for (auto& dly : delay_lines)
        {
            dly->set_modulation_frequency(freq, freq_max);
        }
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    inline void randomize() noexcept
    {
        for (auto& dl : delay_lines)
        {
            dl->randomize();
        }
    }

    inline void reset() noexcept
    {
        for (auto& dl : delay_lines)
        {
            dl->reset();
        }
    }

    inline void process_sample(const real_type* input, real_type* output) noexcept
    {
        *output = process_mono_sample(*input);
    }

  private:
    real_type process_mono_sample(real_type input) noexcept;
};

} // namespace audio_toolbox
