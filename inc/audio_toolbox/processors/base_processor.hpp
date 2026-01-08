#ifndef _AUDIO_TOOLBOX_BASE_PROCESSOR_H_
#define _AUDIO_TOOLBOX_BASE_PROCESSOR_H_

#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"

#define _BASE_PROCESSOR_TYPE BaseProcessor<P, IN_CHANNELS, OUT_CHANNELS, T>
#define _BASE_PROCESSOR_TEMPLATE \
  template <typename P, uint32_t IN_CHANNELS, uint32_t OUT_CHANNELS, typename T>

#define _MONO_PROCESSOR_TYPE MonoProcessor<P, T>
#define _MONO_PROCESSOR_TEMPLATE template <typename P, typename T>

#define _MONO_TO_MULTI_TYPE MonoToMulti<CHANNELS, P, T>
#define _MONO_TO_MULTI_TEMPLATE \
  template <uint32_t CHANNELS, typename P, typename T>

namespace audio_toolbox {

/**
 * @brief Base template class for audio processors. We pass explicitely the
 * processor subclass as a template parameter to avoid the need of virtual
 * functions. If you need to subclass the child class you need to implement
 * virtual functions.
 *
 * @tparam P Processor class that inherits from this class.
 * @tparam IN_CHANNELS Number of input channels.
 * @tparam OUT_CHANNELS Number of output channels.
 * @tparam T Type of the audio samples. Default is real_type.
 */
template <typename P, uint32_t IN_CHANNELS, uint32_t OUT_CHANNELS = IN_CHANNELS,
          typename T = real_type>
class BaseProcessor {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(BaseProcessor);

  BaseProcessor() = default;

  /**
   * @brief Apply the process_sample method to each sample of the input
   * buffer. The subclass must implement the process_sample method.
   *
   * @param input Input buffer (length, IN_CHANNELS).
   * @param output Output buffer (length, OUT_CHANNELS).
   * @param length Per-channel length of the input buffer.
   */
  inline void process(const T* input, T* output, uint32_t length) noexcept;
};

/**
 * @brief Base template class for mono audio processors. We pass explicitely the
 * processor subclass as a template parameter to avoid the need of virtual
 * functions. If you need to subclass the child class you need to implement
 * virtual functions.
 *
 * @tparam P Processor class that inherits from this class.
 * @tparam T Type of the audio samples. Default is real_type.
 */
template <typename P, typename T = real_type>
class MonoProcessor {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(MonoProcessor);

  MonoProcessor() = default;

  /**
   * @brief Apply the process_sample method to each sample of the input
   * buffer. The subclass must implement the process_sample method.
   *
   * @param input Input buffer.
   * @param output Output buffer.
   * @param length Length of the input buffer.
   */
  inline void process(const T* input, T* output, uint32_t length) noexcept;
};

/**
 * @brief Base stereo processor.
 *
 * @tparam P Type of the processor.
 * @tparam T Type of the audio samples.
 */
template <typename P, typename T = real_type>
using StereoProcessor = BaseProcessor<P, 2, 2, T>;

/**
 * @brief Generate a multi-channel processor from a mono processor.
 *
 * @tparam CHANNELS Number of output channels.
 * @tparam P Type of the processor.
 * @tparam T Type of the audio samples.
 */
template <uint32_t CHANNELS, typename P, typename T = real_type>
class MonoToMulti {
  std::array<P, CHANNELS> blocks;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(MonoToMulti);

  MonoToMulti() = default;

  inline void reset() noexcept;

  /**
   * @brief Access the processor at the given index.
   *
   * @param index Index of the processor.
   * @return Processor at the given index.
   */
  inline P& operator[](uint32_t index) noexcept;

  /**
   * @brief First block.
   *
   * @return First block.
   */
  inline auto begin() noexcept;

  /**
   * @brief Last block.
   *
   * @return Last block.
   */
  inline auto end() noexcept;

  /**
   * @brief Process a single sample.
   *
   * @param input Input sample.
   * @param output Output buffer (CHANNELS).
   */
  inline void process_sample(const T* input, T* output) noexcept;

  /**
   * @brief Process a batch of samples.
   *
   * @param input Input buffer (length).
   * @param output Output buffer (length, CHANNELS).
   * @param length Length of the input buffer.
   */
  inline void process(const T* input, T* output, uint32_t length) noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/processors/base_processor_imp.hpp"

#endif  // _AUDIO_TOOLBOX_BASE_PROCESSOR_H_