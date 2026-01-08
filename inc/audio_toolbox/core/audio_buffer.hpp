#ifndef _AUDIO_TOOLBOX_AUDIO_BUFFER_H_
#define _AUDIO_TOOLBOX_AUDIO_BUFFER_H_

#include <complex>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"

#define _AUDIO_BUFFER_TYPE AudioBuffer<LEN, CHANNELS, T>
#define _AUDIO_BUFFER_TEMPLATE \
  template <uint32_t LEN, uint32_t CHANNELS, typename T>

#define _SEEKABLE_AUDIO_BUFFER_TYPE \
  SeekableAudioBuffer<LEN, BLOCK_LEN, CHANNELS, T>
#define _SEEKABLE_AUDIO_BUFFER_TEMPLATE \
  template <uint32_t LEN, uint32_t BLOCK_LEN, uint32_t CHANNELS, typename T>

namespace audio_toolbox {

/**
 * @brief Abstraction of a buffer of audio samples.
 * The channels are interleaved.
 *
 * @note Right now only std::complex<real_type> and real_type are supported.
 * @tparam LEN Per-sample length of the buffer.
 * @tparam CHANNELS Number of channels. Default is 1.
 * @tparam T Type of the samples. Default is real_type.
 */
template <uint32_t LEN, uint32_t CHANNELS = 1, typename T = real_type>
class AudioBuffer {
  static_assert(CHANNELS > 0, "Number of channels must be greater than 0");
  static_assert(LEN > 0, "Length of the buffer must be greater than 0");

  T* buffer;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(AudioBuffer);

  /**
   * @brief Construct a new Audio Buffer object.
   *
   * @note The buffer will be allocated using `pffft_aligned_malloc`.
   */
  AudioBuffer();
  /**
   * @brief Destroy the Audio Buffer object
   *
   */
  ~AudioBuffer();

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
  inline void copy_from_channel(AudioBuffer<LEN, 1, T>& mono_buffer,
                                uint32_t channel) noexcept;

  /**
   * @brief Copy a mono buffer to a specific channel.
   *
   * @note The len of the mono buffer is not checked for performance reasons.
   * @param mono_buffer Source buffer. Should have the same len as the target.
   * @param channel Channel to copy to.
   */
  inline void copy_to_channel(AudioBuffer<LEN, 1, T>& mono_buffer,
                              uint32_t channel) noexcept;

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
  void pffft_aligned_free(void* p) {
    if (p) free(*((void**)p - 1));
  }

/* SSE and co like 16-bytes aligned pointers */
#define MALLOC_V4SF_ALIGNMENT \
  64  // with a 64-byte alignment, we are even aligned on L2 cache lines...
  void* pffft_aligned_malloc(size_t nb_bytes) {
    void *p, *p0 = malloc(nb_bytes + MALLOC_V4SF_ALIGNMENT);
    if (!p0) return (void*)0;
    p = (void*)(((size_t)p0 + MALLOC_V4SF_ALIGNMENT) &
                (~((size_t)(MALLOC_V4SF_ALIGNMENT - 1))));
    *((void**)p - 1) = p0;
    return p;
  }

  // ==============================================================================
};

/**
 * @brief Audio buffer that can be seeked.
 *
 * @tparam LEN Length of the buffer.
 * @tparam BLOCK_LEN Processing block length.
 * @tparam CHANNELS Number of channels. By default is 1.
 * @tparam T Type of the samples. By default is real_type.
 */
template <uint32_t LEN, uint32_t BLOCK_LEN, uint32_t CHANNELS = 1,
          typename T = real_type>
class SeekableAudioBuffer : public AudioBuffer<LEN, CHANNELS, T> {
  uint32_t loop_len = LEN;
  uint32_t samples_left = LEN;
  const T* seek_ptr;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(SeekableAudioBuffer);

  /**
   * @brief Construct a new Seekable Audio Buffer object.
   */
  SeekableAudioBuffer();

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
                                    uint32_t len, uint32_t n_CHANNELS) noexcept;

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
inline void audio_buffer_to_complex(
    AudioBuffer<2 * LEN, CHANNELS, real_type>& interleaved_buffer,
    AudioBuffer<LEN, CHANNELS, std::complex<real_type>>&
        complex_buffer) noexcept;
/**
 * @brief Convert a buffer of complex samples to a buffer
 * of interleaved real-imag samples.
 *
 * @param complex_buffer Output complex buffer.
 * @param interleaved_buffer Interleaved real-imag buffer.
 * @param len Length of the complex buffer.
 * @param n_CHANNELS Number of channels.
 */
inline void audio_buffer_from_complex(
    real_type* interleaved_buffer,
    const std::complex<real_type>* complex_buffer, uint32_t len,
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
inline void audio_buffer_from_complex(
    AudioBuffer<2 * LEN, CHANNELS, real_type>& interleaved_buffer,
    AudioBuffer<LEN, CHANNELS, std::complex<real_type>>&
        complex_buffer) noexcept;

}  // namespace audio_toolbox

#include "audio_toolbox/core/audio_buffer_imp.hpp"

#endif  // _AUDIO_TOOLBOX_AUDIO_BUFFER_H_