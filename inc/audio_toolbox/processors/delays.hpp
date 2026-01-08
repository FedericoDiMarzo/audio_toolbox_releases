#ifndef _AUDIO_TOOLBOX_DELAYS_H_
#define _AUDIO_TOOLBOX_DELAYS_H_

#include <cstdint>

#include "audio_toolbox/core/circular_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/oscillators.hpp"

#define _INTEGER_DELAY_TYPE IntegerDelay<MAX_DLY>
#define _INTEGER_DELAY_TEMPLATE template <uint32_t MAX_DLY>

#define _FRACTIONAL_DELAY_TYPE FractionalDelay<MAX_DLY>
#define _FRACTIONAL_DELAY_TEMPLATE template <uint32_t MAX_DLY>

#define _MODULATED_FRACTIONAL_DELAY_TYPE ModulatedFractionalDelay<MAX_DLY, LFO>
#define _MODULATED_FRACTIONAL_DELAY_TEMPLATE \
  template <uint32_t MAX_DLY, typename LFO>

namespace audio_toolbox {

/**
 * @brief Delay a signal of an integer number of samples.
 *
 * @tparam MAX_DLY Maximum delay in samples (plus one). If MAX_DLY is 10, the
 * maximum delay can be set to 9 samples.
 */
template <uint32_t MAX_DLY>
class IntegerDelay : public MonoProcessor<IntegerDelay<MAX_DLY>> {
  uint32_t delay;
  uint32_t delay_min = 0.0f;
  uint32_t delay_max = MAX_DLY - 1;
  CircularBuffer<MAX_DLY> buffer;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(IntegerDelay);

  /**
   * @param delay Delay in samples.
   */
  IntegerDelay(uint32_t delay);

  IntegerDelay();

  inline void reset() noexcept;

  inline real_type process_sample(real_type input) noexcept;

  /**
   * @brief Get the circular buffer.
   *
   * @return Circular buffer.
   */
  inline CircularBuffer<MAX_DLY>& get_buffer() noexcept;

  /**
   * @brief Set the delay amount.
   */
  inline void set_delay(uint32_t delay) noexcept;

  /**
   * @brief Set a random delay between delay_min and delay_max.
   */
  inline void randomize() noexcept;

  /**
   * @brief Set the delay range for randomization.
   *
   * @param min Minimum delay in samples.
   * @param max Maximum delay in samples.
   */
  void set_random_range(real_type min, real_type max) noexcept;

  /**
   * @brief Get the delay amount.
   *
   * @return Delay amount.
   */
  inline uint32_t get_delay() const noexcept;
};

/**
 * @brief Fractional delay implemented as a linear interpolation
 * between two integer delays.
 *
 * @tparam MAX_DLY Maximum delay in samples (plus one). If MAX_DLY is 10, the
 * maximum delay can be set to 9 samples.
 */
template <uint32_t MAX_DLY>
class FractionalDelay : public MonoProcessor<FractionalDelay<MAX_DLY>> {
  uint32_t delay_int;
  real_type delay_dec;
  real_type delay_min = 0.0f;
  real_type delay_max = MAX_DLY - 1;
  IntegerDelay<MAX_DLY> delay_line;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(FractionalDelay);

  /**
   * @param delay Fractional delay in samples.
   */
  FractionalDelay(real_type delay);

  FractionalDelay();

  inline void reset() noexcept;

  real_type process_sample(real_type input) noexcept;

  /**
   * @brief Get the amount of fractional delay.
   *
   * @return Fractional delay in samples.
   */
  inline real_type get_delay() const noexcept;

  /**
   * @brief Set a random fractional delay between delay_min and delay_max.
   */
  inline void randomize() noexcept;

  /**
   * @brief Set the fractional delay range for randomization.
   *
   * @param min Minimum delay in samples.
   * @param max Maximum delay in samples.
   */
  void set_random_range(real_type min, real_type max) noexcept;

  /**
   * @brief Set the fractional delay.
   *
   * @param delay Delay in samples between 0 and MAX_DLY-1
   */
  inline void set_delay(real_type delay) noexcept;
};

/**
 * @brief A modulated fractional delay line.
 *
 * @tparam MAX_DLY Maximum delay in samples (plus one).
 * @tparam LFO Subclass of LookupOscillator used for modulation.
 */
template <uint32_t MAX_DLY, typename LFO>
class ModulatedFractionalDelay
    : public MonoProcessor<ModulatedFractionalDelay<MAX_DLY, LFO>> {
  real_type depth;
  real_type base_delay_amt;
  FractionalDelay<MAX_DLY> delay_line;
  LFO lfo{[](real_type x) { return std::sin(x); }, 0.0f, M_PI_2, 0.1f};

 public:
  AT_DISABLE_COPY_AND_MOVE_M(ModulatedFractionalDelay);

  /**
   * @param delay Base delay in samples.
   * @param frequency Frequency of the modulation in Hz.
   * @param depth Depth of the modulation in samples.
   */
  ModulatedFractionalDelay(real_type delay, real_type frequency = 0.1,
                           real_type depth = 0.5);

  ModulatedFractionalDelay();

  void reset() noexcept;

  real_type process_sample(real_type input) noexcept;

  /**
   * @brief Set the delay amount.
   *
   * @param delay Delay in samples.
   */
  void set_delay(real_type delay) noexcept;

  /**
   * @brief Set the modulation frequency in Hz.
   *
   * @param frequency Frequency in Hz.
   */
  void set_frequency(real_type frequency) noexcept;

  /**
   * @brief Set the modulation phase.
   *
   * @param phase Phase in radians.
   */
  void set_phase(real_type phase) noexcept;

  /**
   * @brief Set the modulation depth in samples.
   *
   * @param depth Modulation depth in samples.
   */
  inline void set_depth(real_type depth) noexcept;

  /**
   * @brief Set the fractional delay range for randomization.
   *
   * @param min Minimum delay in samples.
   * @param max Maximum delay in samples.
   */
  void set_random_range(real_type min, real_type max) noexcept;

  /**
   * @brief Randomize the delay line delay.
   */
  void randomize() noexcept;

 private:
  void modulate_delay() noexcept;
};

/**
 * @brief Multi-channel integer delay.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam MAX_DLY Maximum delay in samples (plus one).
 */
template <uint32_t CHANNELS, uint32_t MAX_DLY>
using MultiIntegerDelay = MonoToMulti<CHANNELS, IntegerDelay<MAX_DLY>>;

/**
 * @brief Multi-channel fractional delay.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam MAX_DLY Maximum delay in samples (plus one).
 */
template <uint32_t CHANNELS, uint32_t MAX_DLY>
using MultiFractionalDelay = MonoToMulti<CHANNELS, FractionalDelay<MAX_DLY>>;

/**
 * @brief Multi-channel modulated fractional delay.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam MAX_DLY Maximum delay in samples (plus one).
 * @tparam LFO Subclass of LookupOscillator used for modulation.
 */
template <uint32_t CHANNELS, uint32_t MAX_DLY, typename LFO>
using MultiModulatedFractionalDelay =
    MonoToMulti<CHANNELS, ModulatedFractionalDelay<MAX_DLY, LFO>>;

}  // namespace audio_toolbox

#include "audio_toolbox/processors/delays_imp.hpp"

#endif  // _AUDIO_TOOLBOX_DELAYS_H_