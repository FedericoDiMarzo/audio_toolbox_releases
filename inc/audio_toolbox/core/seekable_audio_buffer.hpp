#pragma once

#include <complex>
#include <cstdint>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Audio buffer that can be seeked and looped.
 */
class SeekableAudioBuffer : public AudioBuffer
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    uint32_t loop_len;
    uint32_t samples_left;
    const real_type* seek_ptr;
    uint32_t update_block_len;

    // Construction/Move // == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_M(SeekableAudioBuffer);

    /**
     * @param num_samples Number of samples per channel.
     * @param num_channels Number of channels.
     * @param update_block_len Number of samples considered to update the seek
     * pointer when calling update().
     */
    SeekableAudioBuffer(uint32_t num_samples, uint32_t num_channels = 1, uint32_t update_block_len = 1);
    SeekableAudioBuffer(SeekableAudioBuffer&& other) = default;
    SeekableAudioBuffer& operator=(SeekableAudioBuffer&& other) noexcept = default;
    ~SeekableAudioBuffer() = default;

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    inline const real_type* get_seek_ptr() const noexcept
    {
        return seek_ptr;
    }

    inline uint32_t get_update_block_len() const noexcept
    {
        return update_block_len;
    }

    inline uint32_t get_samples_left() const noexcept
    {
        return samples_left;
    }

    inline void set_loop_len(uint32_t loop_len) noexcept
    {
        this->loop_len = loop_len;
    }

    // Copy // == == == == == == == == == == == == == == == == == == == == == ==
    /**
     * @brief Copy the current update block to a raw buffer.
     *
     * @param output Raw buffer to copy to. Should have the same len as the
     * block.
     */
    void copy_block_to(real_type* output) noexcept;

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    void reset();

    /**
     * @brief Update the seek pointer to the next block.
     *
     * @note When the end of the buffer is reached the seek pointer is reset.
     */
    // TODO: Crossfade when looping back
    void update();
};

} // namespace audio_toolbox
