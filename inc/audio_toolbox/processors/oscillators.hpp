#ifndef _AUDIO_TOOLBOX_OSCILLATORS_H_
#define _AUDIO_TOOLBOX_OSCILLATORS_H_

#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/lookup_table.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/base_processor.hpp"

#define _BASE_OSCILLATOR_TYPE BaseOscillator<SAMPLE_RATE>
#define _BASE_OSCILLATOR_TEMPLATE template <uint32_t SAMPLE_RATE>

#define _LOOKUP_OSCILLATOR_TYPE LookupOscillator<SAMPLE_RATE, SIZE>
#define _LOOKUP_OSCILLATOR_TEMPLATE \
  template <uint32_t SAMPLE_RATE, uint32_t SIZE>
namespace audio_toolbox {

/**
 * @brief Abstract base class for oscillators.
 *
 * @tparam SAMPLE_RATE Sample rate of the audio system.
 */
template <uint32_t SAMPLE_RATE>
class BaseOscillator : public MonoProcessor<BaseOscillator<SAMPLE_RATE>> {
 protected:
  real_type sample_rate;
  real_type frequency;
  real_type phase;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(BaseOscillator);

  /**
   * @brief Construct a new Base Oscillator object.
   *
   * @param frequency Frequency of the oscillator in Hz.
   * @param normalized_phase Initial phase of the oscillator (between 0 and
   * 1).
   */
  BaseOscillator(real_type frequency = SAMPLE_RATE / 2,
                 real_type normalized_phase = 0.0f);

  virtual void reset() noexcept {};

  virtual real_type process_sample() noexcept = 0;

  /**
   * @brief Set the frequency of the oscillator.
   *
   * @param frequency Frequency in Hz.
   */
  virtual void set_frequency(real_type frequency) noexcept;

  /**
   * @brief Get the frequency of the oscillator.
   *
   * @return Frequency in Hz.
   */
  inline real_type get_frequency() const noexcept;

  /**
   * @brief Set the phase of the oscillator.
   *
   * @param phase Normalized phase (between 0 and 1).
   */
  virtual void set_phase(real_type phase) noexcept;
};

/**
 * @brief Oscillator based on a lookup table.
 *
 * @tparam SAMPLE_RATE Sample rate of the audio system.
 * @tparam SIZE Size of the lookup table.
 */
template <uint32_t SAMPLE_RATE, uint32_t SIZE>
class LookupOscillator : public BaseOscillator<SAMPLE_RATE> {
  LookupTable<SIZE> lut;
  real_type table_idx;
  real_type table_idx_increment;

  using super = BaseOscillator<SAMPLE_RATE>;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(LookupOscillator);

  /**
   * @brief Construct a new Lookup Oscillator object.
   *
   * @param function Function to store in the table.
   * @param arg_min Minimum argument of the function.
   * @param arg_max Maximum argument of the function.
   * @param frequency Frequency of the oscillator in Hz.
   * @param normalized_phase Initial phase of the oscillator (between 0 and
   * 1).
   */
  LookupOscillator(std::function<float(float)> function, real_type arg_min,
                   real_type arg_max, real_type frequency = 1.0f,
                   real_type normalized_phase = 0.0f);

  void reset() noexcept override;

  void set_frequency(real_type frequency) noexcept override;

  real_type process_sample() noexcept override;
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/oscillators_imp.hpp"

#endif  // _AUDIO_TOOLBOX_OSCILLATORS_H_