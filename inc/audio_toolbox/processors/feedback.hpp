#ifndef _AUDIO_TOOLBOX_FEEDBACK_H_
#define _AUDIO_TOOLBOX_FEEDBACK_H_

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/matrix.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/delays.hpp"

#define _FEEDBACK_TEMPLATE template <uint32_t CHANNELS, uint32_t MAX_DLY>
#define _FEEDBACK_TYPE Feedback<CHANNELS, MAX_DLY>

namespace audio_toolbox {

/**
 * @brief Feedback processor.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam MAX_DLY Max delay amount in samples.
 */
template <uint32_t CHANNELS, uint32_t MAX_DLY>
class Feedback : public BaseProcessor<_FEEDBACK_TYPE, CHANNELS> {
  MultiIntegerDelay<CHANNELS, MAX_DLY> delay;
  std::array<real_type, CHANNELS> state;
  real_type feedback_gain = 0.5f;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(Feedback);

  Feedback() = default;

  /**
   * @brief Set the feedback gain.
   *
   * @param gain Feedback gain between 0 and 1.
   */
  inline void set_feedback_gain(real_type gain) noexcept;

  inline void reset() noexcept;

  void process_sample(const real_type* input, real_type* output) noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/feedback_imp.hpp"

#endif  // _AUDIO_TOOLBOX_FEEDBACK_H_