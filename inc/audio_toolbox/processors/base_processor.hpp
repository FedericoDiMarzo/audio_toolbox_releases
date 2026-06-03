#pragma once

#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Base template class for audio processors. We pass explicitely the
 * processor subclass as a template parameter to avoid the need of virtual
 * functions. If you need to subclass the child class you need to implement
 * virtual functions.
 *
 * The child class must implement the process_sample method, which processes a
 * single sample of the input buffer. The process method applies the
 * process_sample method to each sample of the input buffer.
 *
 * Process sample signature:
 * void process_sample(const real_type* input, real_type* output) noexcept;
 *
 * @tparam P Processor class that inherits from this class.
 */
template <typename PROCESSOR>
class BaseProcessor
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==
    uint32_t in_channels;
    uint32_t out_channels;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    /**
     * @param in_channels Number of input channels.
     * @param out_channels Number of output channels.
     */
    BaseProcessor(uint32_t in_channels = 1, uint32_t out_channels = 1)
        : in_channels(in_channels),
          out_channels(out_channels)
    {
    }

    // Getters // == == == == == == == == == == == == == == == == == == == == ==

    inline uint32_t get_num_in_channels() const noexcept
    {
        return in_channels;
    }

    inline uint32_t get_num_out_channels() const noexcept
    {
        return out_channels;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Apply the process_sample method to each sample of the input
     * buffer. The subclass must implement the process_sample method.
     *
     * @param input Input buffer (length, in_channels).
     * @param output Output buffer (length, repetitions * out_channels).
     * @param num_samples Per-channel length of the input buffer.
     */
    inline void process(const real_type* input, real_type* output, uint32_t num_samples) noexcept
    {
        const real_type* x = input;
        real_type* y = output;

        // Static polymorphism: call the process_sample method of the child class.
        for (uint32_t t = 0; t < num_samples; t++)
        {
            static_cast<PROCESSOR*>(this)->process_sample(x, y);
            x += in_channels;
            y += out_channels;
        }
    }
};

} // namespace audio_toolbox
