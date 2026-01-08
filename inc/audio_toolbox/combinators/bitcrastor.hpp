#ifndef _AUDIO_TOOLBOX_BITCRASTOR_PROCESSOR_H_
#define _AUDIO_TOOLBOX_BITCRASTOR_PROCESSOR_H_

#include <array>
#include <cstdint>
#include <memory>

#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/bitcrush.hpp"
#include "audio_toolbox/processors/virtual_analog_filters.hpp"

#define _BIT_CRASTOR_TEMPLATE template <uint32_t SAMPLE_RATE>
#define _BIT_CRASTOR_TYPE BitCrastor<SAMPLE_RATE>

namespace audio_toolbox {

/**
 * @brief Combines bitcrushing and filtering to create a lo-fi audio effect.
 *
 * The bitcrusher is controlled by a single parameter mapped to the bit depth,
 * downsample factor, and clip threshold. The bitcrusher can be placed before
 * or after the filter. The filter can be toggled on and off, and the filter
 * type can be changed between four-pole ladder lowpass and highpass filters.
 *
 * @tparam SAMPLE_RATE Sample rate of the audio signals.
 */
template <uint32_t SAMPLE_RATE>
class BitCrastor : public MonoProcessor<_BIT_CRASTOR_TYPE> {
  std::array<std::unique_ptr<AudioFilterBase<SAMPLE_RATE>>, 2> filters = {
      std::make_unique<VALowpassLadder4P<SAMPLE_RATE>>(),
      std::make_unique<VAHighpassLadder4P<SAMPLE_RATE>>()};
  BitCrush bitcrusher;
  real_type gain;
  bool enable_processing = true;
  bool enable_filters = false;
  bool enable_secondary_filter = false;
  bool enable_bitcrusher = false;
  bool bitcrusher_post_filter = false;
  real_type gain_on = 1.7f;
  std::array<real_type, 2> bitdepth_range = {17.0f, 24.0f};
  std::array<real_type, 2> downsample_range = {1.0f, 4.0f};
  std::array<real_type, 2> cb_clip_range = {1.0f, 0.8f};
  std::array<real_type, 2> cb_gain_range = {1.0f, 1.0f};

 public:
  AT_DISABLE_COPY_AND_MOVE_M(BitCrastor);

  BitCrastor();

  inline void reset() noexcept;

  inline real_type process_sample(real_type input) noexcept;

  /**
   * @brief Set the filters frequency.
   *
   * @param freq New frequency value in Hz.
   */
  inline void set_frequency(real_type freq) noexcept;

  /**
   * @brief Set the filters resonance.
   *
   * @param res New resonance value.
   */
  inline void set_resonance(real_type res) noexcept;

  /**
   * @brief Set the bitcrusher amount.
   *
   * @param amount New amount value. Between 0.0 and 1.0.
   */
  inline void set_bitcrush_amount(real_type amount) noexcept;

  /**
   * @brief Enable or disable the processing.
   */
  inline void toggle_processing() noexcept;

  /**
   * @brief Enable or disable the filters.
   */
  inline void toggle_filters() noexcept;

  /**
   * @brief Switch between the lowpass and highpass filters.
   */
  inline void toggle_filter_type() noexcept;

  /**
   * @brief Enable or disable the bitcrusher.
   */
  inline void toggle_bitcrusher() noexcept;

  /**
   * @brief Switch between the bitcrusher before or after the filter.
   */
  inline void toggle_bitcrusher_post_filter() noexcept;

  /**
   * @brief Indicates if the processing is enabled.
   */
  inline bool is_processing_enabled() const noexcept;

  /**
   * @brief Indicates if the bitcrusher is enabled.
   */
  inline bool is_bitcrusher_enabled() const noexcept;

  /**
   * @brief Indicates if the bitcrusher is post filter.
   */
  inline bool is_bitcrusher_post_filter() const noexcept;

  /**
   * @brief Indicates if the filters are enabled.
   */
  inline bool are_filters_enabled() const noexcept;

  /**
   * @brief Indicates if the Highpass filter is enabled.
   */
  inline bool is_secondary_filter_enabled() const noexcept;

 private:
  /**
   * @brief Applies the filters to the input audio sample.
   *
   * @param input Input audio sample.
   * @return Sample processed by the filters.
   */
  inline real_type process_filters(real_type input) noexcept;

  /**
   * @brief Applies the bitcrusher to the input audio sample.
   *
   * @param input Input audio sample.
   * @return Sample processed by the bitcrusher.
   */
  inline real_type process_bitcrusher(real_type input) noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/combinators/bitcrastor_imp.hpp"

#endif  //_AUDIO_TOOLBOX_BITCRASTOR_PROCESSOR_H_