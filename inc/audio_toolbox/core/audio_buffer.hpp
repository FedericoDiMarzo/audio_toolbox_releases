#pragma once

#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Buffer containing interleaved audio samples.
 */
class AudioBuffer
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    real_type* buffer;
    uint32_t num_samples;
    uint32_t num_channels;

    // Construction/Move // == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_M(AudioBuffer);

    /**
     * @param num_samples Number of samples per channel.
     * @param num_channels Number of channels.
     */
    AudioBuffer(uint32_t num_samples, uint32_t num_channels = 1);
    AudioBuffer(AudioBuffer&& other);
    AudioBuffer& operator=(AudioBuffer&& other) noexcept;
    ~AudioBuffer();

    // Getters // == == == == == == == == == == == == == == == == == == == == ==

    inline uint32_t get_num_samples() const noexcept
    {
        return num_samples;
    }

    inline uint32_t get_num_channels() const noexcept
    {
        return num_channels;
    }

    inline uint32_t size() const noexcept
    {
        return num_samples * num_channels;
    }

    // R/W Access // == == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Write access to the underlying buffer.
     *
     * @param index Index of the buffer.
     * @return Reference to the buffer at the given index.
     */
    inline real_type& operator[](uint32_t index) noexcept
    {
        return buffer[index];
    }

    // Begin and end iterators wrap the raw buffer.
    inline real_type* begin() noexcept;
    inline const real_type* begin() const noexcept;
    inline real_type* end() noexcept;
    inline const real_type* end() const noexcept;

    // Raw pointers.
    inline const real_type* get_read_ptr() const noexcept
    {
        return buffer;
    }

    inline real_type* get_write_ptr() const noexcept
    {
        return buffer;
    }

    // Copy // == == == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Copy the content of the buffer to a raw buffer.
     *
     * @param output Target buffer (num_samples * num_channels).
     */
    void copy_to(real_type* output) noexcept;

    /**
     * @brief Copy the content of a raw buffer to the buffer.
     *
     * @param input Input buffer (num_samples * num_channels).
     */
    void copy_from(const real_type* input) noexcept;

    /**
     * @brief Copy a channel of a buffer to a mono buffer.
     *
     * @param mono_buffer Target buffer. Should have the same num_samples as the
     * source.
     * @param channel Channel to copy.
     */
    void copy_from_channel(AudioBuffer& mono_buffer, uint32_t channel) noexcept;

    /**
     * @brief Copy a mono buffer to a specific channel.
     *
     * @param mono_buffer Source buffer. Should have the same num_samples as the
     * target.
     * @param channel Channel to copy to.
     */
    void copy_to_channel(AudioBuffer& mono_buffer, uint32_t channel) noexcept;

    /**
     * @brief Interleave multiple mono buffers into one.
     *
     * @tparam Args Variadic template for the mono buffers.
     * @param buffers Mono buffers to interleave.
     * They should all have the same num_samples as the target.
     */
    template <typename... Args>
    void interleave_from(Args&... buffers) noexcept;

    // Operations on the buffer // == == == == == == == == == == == == == == == ==

    /**
     * @brief Set all the the values of the buffer to 0.
     */
    void clear() noexcept;

    /**
     * @brief Apply a gain to the buffer.
     *
     * @param gain Linear gain.
     */
    void scale(real_type gain) noexcept;

    /**
     * @brief Apply a gain in dB to the buffer.
     *
     * @param gain_db Gain in dB.
     */
    void scale_db(real_type gain_db) noexcept;

    // Private Methods // == == == == == == == == == == == == == == == == == == ==

  private:
    // Custom aligned allocation for pffft compatibility.
    void pffft_aligned_free(void* p);
    void* pffft_aligned_malloc(size_t nb_bytes);
    void move_operator_implementation(audio_toolbox::AudioBuffer& target, audio_toolbox::AudioBuffer& source);

    // == == == == == == == == == == == == == == == == == == == == == == == == ==
};

// Interleave implementation
template <typename... Args>
void AudioBuffer::interleave_from(Args&... buffers) noexcept
{
    // Checking the number of buffers and their type at compile time.
    static_assert(sizeof...(buffers) > 0, "At least one buffer must be provided.");
    static_assert((std::is_same_v<std::remove_cvref_t<Args>, AudioBuffer> && ...),
                  "All arguments must be of type AudioBuffer.");

    const AudioBuffer* buffer_ptrs[] = { &buffers... };
    uint32_t num_buffers = static_cast<uint32_t>(sizeof...(buffers));
    uint32_t selected_channels = std::min(num_buffers, num_channels);

    for (uint32_t i = 0; i < num_samples; i++)
    {
        for (uint32_t j = 0; j < selected_channels; j++)
        {
            real_type value = buffer_ptrs[j]->get_read_ptr()[i];
            buffer[i * num_channels + j] = value;
        }
    }
}
} // namespace audio_toolbox
