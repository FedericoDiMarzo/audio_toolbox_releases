#pragma once

/*
  Bitcrush algorithm.
 */

#include <cmath>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @class BitCrush
 * @brief A class that performs bit crushing on audio signals.
 *
 * The BitCrush class reduces the bit depth of audio signals, resulting in a
 * lo-fi, distorted sound. It provides methods to process audio data, set the
 * bit depth, and reset the state of the processor.
 */
class BitCrush : public BaseProcessor<BitCrush>
{
    uint8_t bit_depth;
    uint32_t downsample_count;
    uint32_t downsample_factor;
    real_type clip_threshold;
    real_type last_sample;
    real_type next_sample;

  public:
    // By default we prefer to disable copy and move operations
    AT_DISABLE_COPY_AND_MOVE_M(BitCrush);

    /**
     * @brief Constructs a new BitCrush processor with the given bit depth,
     * downsample factor, and clip threshold.
     *
     * @param bit_depth The bit depth for the bit crushing operation.
     * @param downsample_factor The factor for downsampling the output.
     * @param clip_threshold The threshold for clipping the output.
     */
    BitCrush(uint8_t bit_depth = 8, uint32_t downsample_factor = 1, real_type clip_threshold = 1.0f);

    void reset() noexcept;

    void process_sample(const real_type* input, real_type* output) noexcept;

    /**
     * @brief Sets the bit depth for the bit crushing operation.
     *
     * @param bit_depth The desired bit depth.
     */
    void set_bit_depth(uint8_t bit_depth) noexcept;
    /**
     * @brief Sets the factor for downsampling the output.
     *
     * @param downsample_factor The desired downsampling factor.
     */
    void set_downsample_factor(uint32_t downsample_factor) noexcept;

    /**
     * @brief Sets the threshold for clipping the output.
     *
     * @param clip_threshold The desired clipping threshold.
     */
    void set_clip_threshold(real_type clip_threshold) noexcept;
};

} // namespace audio_toolbox
