#pragma once

#include <complex>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"
#define _AUDIO_BUFFER_TYPE     AudioBufferStatic<LEN, CHANNELS, T>
#define _AUDIO_BUFFER_TEMPLATE template <uint32_t LEN, uint32_t CHANNELS, typename T>

namespace audio_toolbox
{

/**
 * @brief Buffer containing interleaved audio samples.
 * This is the static version of the audio buffer, where some parameters
 * need to be known at compile time.
 *
 * @note Right now only std::complex<real_type> and real_type are supported.
 * @tparam LEN Per-sample length of the buffer.
 * @tparam CHANNELS Number of channels. Default is 1.
 * @tparam T Type of the samples. Default is real_type.
 */
template <uint32_t LEN, uint32_t CHANNELS = 1, typename T = real_type>
class AudioBufferStatic
{
    static_assert(CHANNELS > 0, "Number of channels must be greater than 0");
    static_assert(LEN > 0, "Length of the buffer must be greater than 0");

    T* buffer;

  public:
    AT_DISABLE_COPY_AND_MOVE_M(AudioBufferStatic);

    /**
     * @brief Construct a new Audio Buffer object.
     *
     * @note The buffer will be allocated using `pffft_aligned_malloc`.
     */
    AudioBufferStatic();
    /**
     * @brief Destroy the Audio Buffer object
     *
     */
    ~AudioBufferStatic();

    /**
     * @brief Access to the underlying buffer.
     *
     * @param index Index of the buffer.
     * @return Reference to the buffer at the given index.
     */
    inline T& operator[](uint32_t index) noexcept;

    // Begin and end iterators wrap the raw buffer.
    inline T* begin() noexcept;
    inline const T* begin() const noexcept;
    inline T* end() noexcept;
    inline const T* end() const noexcept;

    /**
     * @brief Get the a const pointer to the buffer.
     *
     * @return Read only pointer to the buffer.
     */
    inline const T* get_read_ptr() const noexcept;

    /**
     * @brief Get the a pointer to the buffer.
     *
     * @return Read and write pointer to the buffer.
     */
    inline T* get_write_ptr() const noexcept;

    /**
     * @brief Get the length of the buffer.
     *
     * @return Length of the buffer.
     */
    inline uint32_t get_len() const noexcept;

    /**
     * @brief Get the number of channels.
     *
     * @return Number of channels.
     */
    inline uint32_t get_channels() const noexcept;

    /**
     * @brief Get the total size of the buffer (channels * len).
     *
     * @return Total size of the buffer.
     */
    inline uint32_t size() const noexcept;

    /**
     * @brief Copy the content of the buffer to a raw buffer.
     *
     * @param output Target buffer. Should have the same LEN as the source.
     */
    inline void copy_to(T* output) noexcept;

    /**
     * @brief Copy a channel of a buffer to a mono buffer.
     *
     * @note The len of the mono buffer is not checked for performance reasons.
     * @param mono_buffer Target buffer. Should have the same len as the source.
     * @param channel Channel to copy.
     */
    inline void copy_from_channel(AudioBufferStatic<LEN, 1, T>& mono_buffer, uint32_t channel) noexcept;

    /**
     * @brief Copy a mono buffer to a specific channel.
     *
     * @note The len of the mono buffer is not checked for performance reasons.
     * @param mono_buffer Source buffer. Should have the same len as the target.
     * @param channel Channel to copy to.
     */
    inline void copy_to_channel(AudioBufferStatic<LEN, 1, T>& mono_buffer, uint32_t channel) noexcept;

    /**
     * @brief Interleave multiple buffers into one.
     *
     * @note The len of the buffers is not checked for performance reasons.
     * @tparam Buffers mono buffers to interleave.
     * @param buffers Mono buffers to interleave.
     * They should all have the same len as the target.
     */
    template <typename... Buffers>
    inline void interleave_from(Buffers&... buffers) noexcept;

    /**
     * @brief Set all the the values of the buffer to 0.
     */
    inline void clear() noexcept;

    /**
     * @brief Apply a gain to the buffer.
     *
     * @param gain Linear gain.
     */
    inline void scale(T gain) noexcept;

    /**
     * @brief Apply a gain in dB to the buffer.
     *
     * @param gain_db Gain in dB.
     */
    inline void scale_db(T gain_db) noexcept;

  private:
    // ==============================================================================

    // TODO: This is a temporary fix for the pffft_aligned_malloc and
    // pffft_aligned_free functions.
    void pffft_aligned_free(void* p)
    {
        if (p)
            free(*((void**)p - 1));
    }

/* SSE and co like 16-bytes aligned pointers */
#define MALLOC_V4SF_ALIGNMENT 64 // with a 64-byte alignment, we are even aligned on L2 cache lines...

    void* pffft_aligned_malloc(size_t nb_bytes)
    {
        void *p, *p0 = malloc(nb_bytes + MALLOC_V4SF_ALIGNMENT);
        if (!p0)
            return (void*)0;
        p = (void*)(((size_t)p0 + MALLOC_V4SF_ALIGNMENT) & (~((size_t)(MALLOC_V4SF_ALIGNMENT - 1))));
        *((void**)p - 1) = p0;
        return p;
    }

    // ==============================================================================
};

/**
 * @brief Convert a buffer of interleaved real-imag samples to a buffer of
 * complex samples.
 *
 * @param interleaved_buffer Interleaved real-imag buffer.
 * @param complex_buffer Output complex buffer.
 * @param len Length of the complex buffer.
 * @param n_CHANNELS Number of channels.
 */
inline void audio_buffer_to_complex(const real_type* interleaved_buffer,
                                    std::complex<real_type>* complex_buffer,
                                    uint32_t len,
                                    uint32_t n_CHANNELS) noexcept;

/**
 * @brief Convert a buffer of interleaved real-imag samples to a buffer of
 * complex samples.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam LEN Length of the buffer.
 * @param interleaved_buffer Interleaved real-imag buffer.
 * @param complex_buffer Output complex buffer.
 */
template <uint32_t CHANNELS, uint32_t LEN>
inline void audio_buffer_to_complex(AudioBufferStatic<2 * LEN, CHANNELS, real_type>& interleaved_buffer,
                                    AudioBufferStatic<LEN, CHANNELS, std::complex<real_type>>& complex_buffer) noexcept;
/**
 * @brief Convert a buffer of complex samples to a buffer
 * of interleaved real-imag samples.
 *
 * @param complex_buffer Output complex buffer.
 * @param interleaved_buffer Interleaved real-imag buffer.
 * @param len Length of the complex buffer.
 * @param n_CHANNELS Number of channels.
 */
inline void audio_buffer_from_complex(real_type* interleaved_buffer,
                                      const std::complex<real_type>* complex_buffer,
                                      uint32_t len,
                                      uint32_t n_CHANNELS) noexcept;

/**
 * @brief Convert a buffer of complex samples to a buffer
 * of interleaved real-imag samples.
 *
 * @tparam CHANNELS Number of channels.
 * @tparam LEN Length of the buffer.
 * @param complex_buffer Output complex buffer.
 * @param interleaved_buffer Interleaved real-imag buffer.
 */
template <uint32_t CHANNELS, uint32_t LEN>
inline void
audio_buffer_from_complex(AudioBufferStatic<2 * LEN, CHANNELS, real_type>& interleaved_buffer,
                          AudioBufferStatic<LEN, CHANNELS, std::complex<real_type>>& complex_buffer) noexcept;

} // namespace audio_toolbox

#include "audio_toolbox/static/audio_buffer_static_imp.hpp"
