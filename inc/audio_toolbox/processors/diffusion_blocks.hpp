#ifndef _AUDIO_TOOLBOX_DIFFUSION_H_
#define _AUDIO_TOOLBOX_DIFFUSION_H_

/*
 * Credits to
 * https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb/
 * https://github.com/Signalsmith-Audio/reverb-example-code
 */

#include <array>
#include <cstdint>
#include <memory>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/matrix.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/delays.hpp"

#define _CHANNEL_EXPANDER_TYPE ChannelExpander<CHANNELS>
#define _CHANNEL_EXPANDER_TEMPLATE template <uint32_t CHANNELS>

#define _MULTI_CHANNEL_EXPANDER_TYPE MultiChannelExpander<CH_IN, CH_OUT>
#define _MULTI_CHANNEL_EXPANDER_TEMPLATE \
  template <uint32_t CH_IN, uint32_t CH_OUT>

#define _CHANNEL_REDUCER_TYPE ChannelReducer<CH_IN, CH_OUT>
#define _CHANNEL_REDUCER_TEMPLATE template <uint32_t CH_IN, uint32_t CH_OUT>

#define _CHANNEL_REDUCER_AVG_TYPE ChannelReducerAvg<CH_IN, CH_OUT>
#define _CHANNEL_REDUCER_AVG_TEMPLATE template <uint32_t CH_IN, uint32_t CH_OUT>

#define _CHANNEL_BLENDER_TYPE ChannelBlender<CHANNELS>
#define _CHANNEL_BLENDER_TEMPLATE template <uint32_t CHANNELS>

#define _DIFFUSION_STEP_TYPE DiffusionStep<CHANNELS, DLY, MIX>
#define _DIFFUSION_STEP_TEMPLATE \
  template <uint32_t CHANNELS, typename DLY, typename MIX>

// We use the interleaved representation for our diffusion pipeline.

namespace audio_toolbox {

/**
 * @brief Expands a mono signal to a multichannel signal.
 *
 * @tparam CHANNELS Number of output channels.
 */
template <uint32_t CHANNELS>
class ChannelExpander {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(ChannelExpander);

  ChannelExpander() = default;

  /**
   * @brief Process a single sample.
   *
   * @param input Input sample.
   * @param output Output buffer (CHANNELS).
   */
  inline void process_sample(real_type input, real_type* output) noexcept;

  /**
   * @brief Process a batch of samples.
   *
   * @param input Input buffer (length).
   * @param output Output buffer (length, CHANNELS).
   * @param length Length of the input buffer.
   */
  inline void process(const real_type* input, real_type* output,
                      uint32_t length) noexcept;
};

/**
 * @brief Expands a multichannel signal to an higher number of channels.
 *
 * @tparam CH_IN Number of input channels.
 * @tparam CH_OUT Number of output channels
 */
template <uint32_t CH_IN, uint32_t CH_OUT>
class MultiChannelExpander
    : public BaseProcessor<MultiChannelExpander<CH_IN, CH_OUT>, CH_IN, CH_OUT> {
  static_assert((CH_OUT % CH_IN) == 0, "CH_OUT should be divisible by CH_IN");

  static constexpr uint32_t GROUP_LEN = CH_OUT / CH_IN;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(MultiChannelExpander);

  MultiChannelExpander() = default;

  inline void process_sample(const real_type* input,
                             real_type* output) noexcept;
};

/**
 * @brief Reduces a multi-channel signal to a lower number of channels.
 *
 * @tparam CHANNELS Number of input channels.
 * @tparam CHANNELS_OUT Number of output channels.
 */
template <uint32_t CH_IN, uint32_t CH_OUT>
class ChannelReducer {
  static_assert(CH_OUT < CH_IN,
                "Output channels must be less than input channels");
  static_assert(CH_IN % CH_OUT == 0,
                "Input channels must be a multiple of output channels");

  static constexpr uint32_t GROUP_LEN = CH_IN / CH_OUT;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(ChannelReducer);

  ChannelReducer() = default;

  /**
   * @brief Process a single sample.
   *
   * @param input Input sample (CH_IN).
   * @param output Output sample (CH_OUT).
   */
  inline void process_sample(const real_type* input,
                             real_type* output) noexcept;

  /**
   * @brief Process a batch of samples.
   *
   * @param input Input buffer (length, CH_IN).
   * @param output Output buffer (length, CH_OUT).
   * @param length Length of the input buffers.
   */
  inline void process(const real_type* input, real_type* output,
                      uint32_t length) noexcept;
};

/**
 * @brief Mixes a multi-channel signal to a lower number of channels.
 *
 * @tparam CHANNELS Number of input channels.
 * @tparam CHANNELS_OUT Number of output channels.
 */
template <uint32_t CH_IN, uint32_t CH_OUT>
class ChannelReducerAvg {
  static_assert(CH_OUT < CH_IN,
                "Output channels must be less than input channels");
  static_assert(CH_IN % CH_OUT == 0,
                "Input channels must be a multiple of output channels");

  static constexpr uint32_t GROUP_LEN = CH_IN / CH_OUT;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(ChannelReducerAvg);

  ChannelReducerAvg() = default;

  /**
   * @brief Process a single sample.
   *
   * @param input Input sample (CH_IN).
   * @param output Output sample (CH_OUT).
   */
  inline void process_sample(const real_type* input,
                             real_type* output) noexcept;

  /**
   * @brief Process a batch of samples.
   *
   * @param input Input buffer (length, CH_IN).
   * @param output Output buffer (length, CH_OUT).
   * @param length Length of the input buffers.
   */
  inline void process(const real_type* input, real_type* output,
                      uint32_t length) noexcept;
};
/**
 * @brief Blends the channels of a multichannel signal.
 *
 * @tparam CHANNELS Number of channels.
 */
template <uint32_t CHANNELS>
class ChannelBlender
    : public BaseProcessor<ChannelBlender<CHANNELS>, CHANNELS> {
  HadamardMatrix<CHANNELS, true> matrix;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(ChannelBlender);

  ChannelBlender() = default;

  /**
   * @brief Process a single sample.
   *
   * @param input Input sample (CHANNELS).
   * @param output Output sample (CHANNELS).
   */
  void process_sample(const real_type* input, real_type* output) noexcept;
};

/**
 * @brief Combines a multichannel delay and mixer into a diffusion block.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam DLY Delay type.
 * @tparam MIX Mixer type.
 */
template <uint32_t CHANNELS, typename DLY, typename MIX>
class DiffusionStep
    : public BaseProcessor<DiffusionStep<CHANNELS, DLY, MIX>, CHANNELS> {
  DLY delay;
  MIX mixer;
  std::array<real_type, CHANNELS> buff;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(DiffusionStep);

  DiffusionStep() = default;

  inline void reset() noexcept;

  void process_sample(const real_type* input, real_type* output) noexcept;

  /**
   * @brief Get the delay instance.
   *
   * @return Delay instance.
   */
  inline DLY& get_delay() noexcept;

  /**
   * @brief Get the mixer instance.
   *
   * @return Mixer instance.
   */
  inline MIX& get_mixer() noexcept;
};

/**
 * @brief Diffusion step using fractional delay and Hadamard matrix.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam MAX_DLY Maximum delay in samples.
 */
template <uint32_t CHANNELS, uint32_t MAX_DLY>
using DiffusionStepFractional =
    DiffusionStep<CHANNELS, MultiFractionalDelay<CHANNELS, MAX_DLY>,
                  ChannelBlender<CHANNELS>>;

template <uint32_t CHANNELS, uint32_t MAX_DLY, typename LFO>
using DiffusionStepModulatedFractional =
    DiffusionStep<CHANNELS,
                  MultiModulatedFractionalDelay<CHANNELS, MAX_DLY, LFO>,
                  ChannelBlender<CHANNELS>>;

}  // namespace audio_toolbox

#include "audio_toolbox/processors/diffusion_blocks_imp.hpp"

#endif  // _AUDIO_TOOLBOX_DIFFUSION_H_