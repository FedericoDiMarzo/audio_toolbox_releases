#pragma once

#include <cstdint>

#include "audio_toolbox/core/circular_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/static/constexpr.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Delay a signal of an integer number of samples.
 */
class IntegerDelay : public BaseProcessor<IntegerDelay>
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    uint32_t delay;
    uint32_t delay_max;
    uint32_t delay_rand_min;
    uint32_t delay_rand_max;
    CircularBuffer<real_type> buffer;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(IntegerDelay);

    /**
     * @param max_delay Maximum delay in samples. Set to the next power of
     * two to comply with the circular buffer implementation.
     */
    IntegerDelay(uint32_t max_delay)
        : delay(max_delay),
          delay_max(max_delay),
          delay_rand_min(0),
          delay_rand_max(max_delay),
          buffer(next_power_of_two(max_delay + 1))
    {
    }

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Set the fractional delay range for randomization.
     *
     * @param min Minimum delay in samples.
     * @param max Maximum delay in samples.
     */
    inline void set_random_range(uint32_t min, uint32_t max) noexcept
    {
        delay_rand_min = clip(min, 0, delay_max);
        delay_rand_max = clip(max, delay_rand_min, delay_max);
    }

    inline void set_delay(uint32_t delay) noexcept
    {
        this->delay = clip(delay, delay_rand_min, delay_rand_max);
    }

    inline uint32_t get_delay() const noexcept
    {
        return delay;
    }

    inline CircularBuffer<real_type>& get_buffer() noexcept
    {
        return buffer;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    inline real_type process_mono_sample(const real_type input) noexcept
    {
        buffer.push(input);
        return buffer[delay + 1];
    }

    inline void reset() noexcept
    {
        buffer.clear();
    }

    inline void process_sample(const real_type* input, real_type* output) noexcept
    {
        *output = process_mono_sample(*input);
    };

    /**
     * @brief Call this after set_random_range.
     */
    inline void randomize() noexcept
    {
        set_delay(get_random_int(delay_rand_min, delay_rand_max));
    }
};

} // namespace audio_toolbox