#ifndef _AUDIO_TOOLBOX_CHORUS_H_
#define _AUDIO_TOOLBOX_CHORUS_H_

#include <array>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/delays.hpp"
#include "audio_toolbox/processors/oscillators.hpp"

#define _BASIC_CHORUS_TEMPLATE \
  template <typename LFO, uint32_t NUM_VOICES, uint32_t MAX_DLY>

#define _BASIC_CHORUS_TYPE BasicChorus<LFO, NUM_VOICES, MAX_DLY>

namespace audio_toolbox {

/**
 * @brief Simple mono chorus effect with multiple modulated delayy lines.
 *
 * @tparam LFO Subclass of LookupOscillator.
 * @tparam NUM_VOICES Number of delay lines.
 * @tparam MAX_DLY Maximum delay in samples (plus one) for each delay line.
 */
template <typename LFO, uint32_t NUM_VOICES = 1,
          uint32_t MAX_DLY = next_power_of_two(48000)>
class BasicChorus : public MonoProcessor<_BASIC_CHORUS_TYPE> {
  using Delay = ModulatedFractionalDelay<MAX_DLY, LFO>;

  real_type mix;
  std::array<Delay, NUM_VOICES> delays;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(BasicChorus);

  /**
   * @param frequency Modulation frequency.
   * @param depth Depth of the modulation.
   * @param delay Base delay in samples.
   * @param mix Mix between dry and wet signals.
   */
  BasicChorus(real_type frequency = 0.1, real_type depth = 0.5,
              real_type delay = 300, real_type mix = 0.8);

  inline void reset() noexcept;

  inline real_type process_sample(real_type input) noexcept;

  /**
   * @brief Set the base delay in samples.
   *
   * @param delay New base delay in samples.
   */
  inline void set_delay(real_type delay) noexcept;

  /**
   * @brief Set the modulation frequency in Hz.
   *
   * @param frequency New modulation frequency in Hz.
   */
  inline void set_frequency(real_type frequency) noexcept;

  /**
   * @brief Set the modulation amount in samples.
   *
   * @param depth New modulation amount in samples.
   */
  inline void set_depth(real_type depth) noexcept;

  /**
   * @brief Set the mix between dry and wet signals.
   *
   * @param mix New mix value.
   */
  inline void set_mix(real_type mix) noexcept;

  /**
   * @brief Randomize the phases of the delay lines LFOs.
   */
  inline void randomize_phases() noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/chorus_imp.hpp"

#endif  // _AUDIO_TOOLBOX_CHORUS_H_