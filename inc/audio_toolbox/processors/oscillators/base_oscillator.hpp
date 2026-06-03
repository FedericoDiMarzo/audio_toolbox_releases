#pragma once

#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{


/**
 * @brief Abstract base class for oscillators.
 *
 * Note: you should subclass the following processing method:
 * real_type generate_sample() noexcept;
 */
class BaseOscillator : public BaseProcessor<BaseOscillator>
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==
  protected:
    uint32_t sample_rate;
    real_type initial_phase;
    real_type frequency;
    real_type normalized_phase;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(BaseOscillator);

    /**
     * @brief Construct a new Base Oscillator object.
     *
     * @param sample_rate Sample rate of the audio system in Hz.
     * @param frequency Frequency of the oscillator in Hz.
     * @param normalized_phase Initial phase of the oscillator in [0, 1].
     */
    BaseOscillator(uint32_t sample_rate, real_type frequency, real_type normalized_phase = 0.0f)
        : BaseProcessor(0, 1),
          sample_rate(sample_rate)
    {
        set_frequency(frequency);
        set_phase(normalized_phase);
        set_initial_phase(normalized_phase);
    }

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    virtual inline void set_frequency(real_type frequency) noexcept
    {
        this->frequency = frequency;
    }

    inline real_type get_frequency() const noexcept
    {
        return frequency;
    }

    inline void set_phase(real_type normalized_phase) noexcept
    {
        this->normalized_phase = clip(normalized_phase, 0.0f, 1.0f);
    }

    inline real_type get_phase() const noexcept
    {
        return normalized_phase;
    }

    void set_initial_phase(real_type initial_phase) noexcept
    {
        this->initial_phase = clip(initial_phase, 0.0f, 1.0f);
    }

    inline real_type get_initial_phase() const noexcept
    {
        return initial_phase;
    }

    inline uint32_t get_sample_rate() const noexcept
    {
        return sample_rate;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==
    virtual void reset() noexcept
    {
        normalized_phase = initial_phase;
    };

    inline void process_sample([[maybe_unused]] const real_type* input, real_type* output) noexcept
    {
        *output = generate_sample();
    }

    /**
     * @brief Generate a single sample of the oscillator.
     *
     * @return The generated sample.
     */
    virtual real_type generate_sample() noexcept = 0;
};

} // namespace audio_toolbox