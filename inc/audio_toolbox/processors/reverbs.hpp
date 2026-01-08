#ifndef _AUDIO_TOOLBOX_REVERBS_H_
#define _AUDIO_TOOLBOX_REVERBS_H_

#include <array>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/matrix.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/diffusion_blocks.hpp"
#include "audio_toolbox/processors/feedback.hpp"
#include "audio_toolbox/processors/oscillators.hpp"

#define _REVERB_DIFFUSE_TYPE \
  ReverbDiffuse<SAMPLE_RATE, IN_CH, OUT_CH, DIFF_CH, DIFF_MAX_DLY, FB_MAX_DLY>

#define _REVERB_DIFFUSED_TEMPLATE                                  \
  template <uint32_t SAMPLE_RATE, uint32_t IN_CH, uint32_t OUT_CH, \
            uint32_t DIFF_CH, uint32_t DIFF_MAX_DLY, uint32_t FB_MAX_DLY>

namespace audio_toolbox {

template <uint32_t SAMPLE_RATE, uint32_t IN_CH, uint32_t OUT_CH,
          uint32_t DIFF_CH, uint32_t DIFF_MAX_DLY, uint32_t FB_MAX_DLY>
class ReverbDiffuse : public BaseProcessor<_REVERB_DIFFUSE_TYPE, IN_CH, OUT_CH>

{
  static constexpr uint32_t MAX_DIFF_STEPS = 128;
  static constexpr uint32_t LUT_SIZE = 128;
  static constexpr real_type DLY_RAND_DELTA = DIFF_MAX_DLY / 4;
  static constexpr uint32_t DECAY_MAX = DIFF_MAX_DLY - DLY_RAND_DELTA;
  static constexpr real_type LFO_INIT_FREQ = 0.05f;
  static constexpr real_type LFO_INIT_FREQ_DELTA = 0.03f;
  static constexpr real_type INITIAL_DECAY = 0.3f;

  using LFO = LookupOscillator<SAMPLE_RATE, LUT_SIZE>;
  using Diff = DiffusionStepModulatedFractional<DIFF_CH, DIFF_MAX_DLY, LFO>;

  std::array<Diff, MAX_DIFF_STEPS> diff_blocks;
  Feedback<DIFF_CH, FB_MAX_DLY> feedback;  // TODO: Use fractional delay here
  MultiChannelExpander<IN_CH, DIFF_CH> expander;
  ChannelReducer<DIFF_CH, OUT_CH> reducer;
  std::array<AudioBuffer<1, DIFF_CH>, 4> tmp;
  uint32_t num_steps = MAX_DIFF_STEPS / 2;
  real_type alpha_residual = 0.5f;
  real_type lfo_to_diffusion = 0.0f;
  real_type lfo_to_feedback = 0.0f;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(ReverbDiffuse);

  ReverbDiffuse();
  inline void reset() noexcept;

  void process_sample(const real_type* input, real_type* output);
  /**
   * @brief Set the feedback gain.
   *
   * @param gain Feedback gain between 0 and 1.
   */
  inline void set_feedback_gain(real_type gain) noexcept;

  /**
   * @brief Set the residual diffusion gain.
   *
   * @param alpha Value between 0 and 1.
   */
  inline void set_alpha_residual(real_type alpha) noexcept;
  /**
   * @brief Set the number of diffusion steps.
   *
   * @param steps Number of diffusion steps.
   */
  inline void set_diffusion_steps(uint32_t steps) noexcept;

  /**
   * @brief Set the LFO to diffusion gain.
   *
   * @param lfo_to_fb LFO to feedback gain in samples.
   */
  inline void set_lfo_to_diffusion(real_type lfo_to_diff) noexcept;

  /**
   * @brief Set the center delay in the delay lines.
   *
   * @param decay Value between 0 and 1.
   */
  void set_decay(real_type decay) noexcept;

  /**
   * @brief Set the LFO frequency.
   *
   * @param freq Frequency in Hz.
   * @param delta Random variation between the delay lines.
   */
  void set_lfo_frequency(real_type freq, real_type delta) noexcept;

 private:
  /**
   * @brief Map a function to all dif
   *
   * @tparam DLY ModulatedFractionalDelay type.
   * @tparam FN Function type.
   * @param fn
   */
  template <typename FN>
  void foreach_delay(FN fn);
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/reverbs_imp.hpp"

#endif  // _AUDIO_TOOLBOX_REVERBS_H_