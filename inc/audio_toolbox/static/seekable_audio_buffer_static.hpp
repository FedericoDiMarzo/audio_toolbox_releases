#pragma once

#include <complex>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/audio_buffer_static.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"
#define _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE SeekableAudioBufferStatic<LEN, BLOCK_LEN, CHANNELS, T>
#define _SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE                                                                         \
    template <uint32_t LEN, uint32_t BLOCK_LEN, uint32_t CHANNELS, typename T>

namespace audio_toolbox
{

/**
 * @brief Audio buffer that can be seeked.
 *
 * @tparam LEN Length of the buffer.
 * @tparam BLOCK_LEN Processing block length.
 * @tparam CHANNELS Number of channels. By default is 1.
 * @tparam T Type of the samples. By default is real_type.
 */
template <uint32_t LEN, uint32_t BLOCK_LEN, uint32_t CHANNELS = 1, typename T = real_type>
class SeekableAudioBufferStatic : public AudioBufferStatic<LEN, CHANNELS, T>
{
    uint32_t loop_len = LEN;
    uint32_t samples_left = LEN;
    const T* seek_ptr;

  public:
    AT_DISABLE_COPY_AND_MOVE_M(SeekableAudioBufferStatic);

    /**
     * @brief Construct a new Seekable Audio Buffer object.
     */
    SeekableAudioBufferStatic();

    /**
     * @brief Get the seek pointer.
     *
     * @return Seek pointer.
     */
    inline const T* get_seek_ptr() const noexcept;

    /**
     * @brief Get the block len.
     *
     * @return Block len.
     */
    inline uint32_t get_block_len() const noexcept;

    /**
     * @brief Get the number of samples left.
     *
     * @return Number of samples left.
     */
    inline uint32_t get_samples_left() const noexcept;

    /**
     * @brief Set the lenght of the loop.
     *
     * @note If the loop len is greater than the buffer len the loop len is set
     * to the buffer len.
     *
     * @note The buffer is reset.
     * @param loop_len Length of the loop.
     */
    inline void set_loop_len(uint32_t loop_len) noexcept;

    /**
     * @brief Resets the seek pointer to the beginning of the buffer.
     */
    void reset();

    /**
     * @brief Update the seek pointer to the next block.
     *
     * @note When the end of the buffer is reached the seek pointer is reset.
     */
    // TODO: Crossfade when looping back
    void update();

    /**
     * @brief Copy the current block to a raw buffer.
     *
     * @param output Raw buffer to copy to. Should have the same len as the
     * block.
     */
    inline void copy_block_to(T* output) noexcept;
};

} // namespace audio_toolbox

#include "audio_toolbox/static/seekable_audio_buffer_static_imp.hpp"
