#pragma once

#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/lookup_table.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/oscillators/base_oscillator.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{


class WavetableOscillator : public BaseOscillator
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==
    LookupTable lut;
    real_type table_idx;
    real_type table_idx_increment;

    // Construction // == == == == == == == == == == == == == == == == == == == ==
  public:
    AT_DISABLE_COPY_AND_MOVE_M(WavetableOscillator);

    /**
     * @param lut Defines a full period of the oscillator waveform.
     * @param sample_rate Sample rate of the audio in Hz.
     * @param frequency Frequency of the oscillator in Hz.
     * @param normalized_phase Initial phase of the oscillator in [0, 1].
     */
    WavetableOscillator(LookupTable lut, uint32_t sample_rate, real_type frequency, real_type normalized_phase = 0.0f);
    virtual ~WavetableOscillator() = default;

    // Getters/Setters // == == == == == == == == == == == == == == == == == ==
    void set_frequency(real_type frequency) noexcept override;

    // Processing // == == == == == == == == == == == == == == == == == == == ==
    real_type generate_sample() noexcept override;

    void randomize() noexcept;

    void reset() noexcept override;
};

} // namespace audio_toolbox
