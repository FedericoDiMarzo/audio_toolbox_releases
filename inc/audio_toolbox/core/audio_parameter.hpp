#pragma once

#include <atomic>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Utility class that provides smooth transitions between values
 * in an audio rendering loop.
 */
class AudioParameter
{
    uint32_t sample_rate;
    uint32_t block_size;
    std::atomic<real_type> target_value;
    std::atomic<real_type> current_value;
    real_type alpha;

  public:
    AT_DISABLE_COPY_AND_MOVE_M(AudioParameter);

    /**
     * @brief Construct a new Audio Parameter object.
     *
     * @param value Starting value.
     * @param sample_rate Sample rate of the audio rendering loop.
     * @param block_size Measure the update period of the parameter.
     * @param transition_ms Time constant of the EMA in milliseconds.
     */
    AudioParameter(real_type value, uint32_t sample_rate, uint32_t block_size, float transition_ms);

    /**
     * @brief Update the parameter value.
     *
     * @note This method should be called once per block.
     */
    inline void update() noexcept;

    /**
     * @brief Set a new target value.
     *
     * @param value The new value.
     */
    inline void set(real_type value) noexcept;

    /**
     * @brief Get the current value.
     *
     * @return Current value.
     */
    inline real_type get() const noexcept;

    /**
     * @brief Set the transition time.
     *
     * @param transition_ms Transition time in milliseconds.
     */
    void set_transition_ms(real_type transition_ms);
};

using AudioParameterDispatcher = UpdateDispatcher<AudioParameter>;

} // namespace audio_toolbox

#include "audio_toolbox/core/audio_parameter_imp.hpp"
