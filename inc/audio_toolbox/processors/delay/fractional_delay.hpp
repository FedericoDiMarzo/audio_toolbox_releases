#pragma once

#include <cstdint>
#include <memory>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/delay/integer_delay.hpp"
#include "audio_toolbox/processors/oscillators/wavetable_oscillator.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Fractional delay implemented as a linear interpolation
 * between two integer delays.
 */
class FractionalDelay : public BaseProcessor<FractionalDelay>
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    IntegerDelay delay_line;
    std::unique_ptr<WavetableOscillator> lfo;
    real_type delay;
    real_type delay_rand_min;
    real_type delay_rand_max;
    real_type modulation_depth;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(FractionalDelay);

    /**
     * @param delay Fractional delay in samples.
     * @param lfo Optional LFO for modulating the delay. Use set_modulation_depth
     * to set the modulation depth.
     */
    FractionalDelay(real_type delay, std::unique_ptr<WavetableOscillator> lfo = nullptr);

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Set the fractional delay range for randomization.
     *
     * @param min Minimum delay in samples.
     * @param max Maximum delay in samples.
     */
    void set_random_range(real_type min, real_type max) noexcept
    {
        delay_rand_min = clip(min, 0.0f, delay_rand_max);
        delay_rand_max = clip(max, delay_rand_min, delay_rand_max);
    }

    inline void set_delay(real_type delay) noexcept
    {
        this->delay = clip(delay, delay_rand_min, delay_rand_max);
    }

    inline real_type get_modulation_depth() const noexcept
    {
        return modulation_depth;
    }

    inline void set_modulation_depth(real_type depth) noexcept
    {
        modulation_depth = depth;
    }

    /**
     * @brief Set the modulation frequency. Pass freq_max to
     * randomize the modulation frequency in the range [freq, freq_max].
     * If freq_max is  not specified, the modulation frequency is set to freq.
     *
     * @param freq Target modulation frequency in Hz.
     * @param freq_max If a valid value is passed, the modulation frequency is
     * randomized in the range [freq, freq_max].
     */
    void set_modulation_frequency(real_type freq, real_type freq_max = -1.0f) noexcept;

    real_type get_modulation_frequency() const noexcept;

    inline CircularBuffer<real_type>& get_buffer() noexcept
    {
        return delay_line.get_buffer();
    }

    inline WavetableOscillator& get_lfo() const noexcept
    {
        return *lfo;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    real_type process_mono_sample(real_type input) noexcept;

    inline void reset() noexcept
    {
        delay_line.reset();
    }

    inline void process_sample(const real_type* input, real_type* output) noexcept
    {
        *output = process_mono_sample(*input);
    }

    /**
     * @brief Call this after set_random_range.
     */
    void randomize() noexcept
    {
        set_delay(get_random_real(delay_rand_min, delay_rand_max));
        lfo->randomize();
    }
};

} // namespace audio_toolbox