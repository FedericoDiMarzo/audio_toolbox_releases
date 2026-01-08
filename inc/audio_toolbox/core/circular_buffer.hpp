#ifndef _AUDIO_TOOLBOX_CIRCULAR_BUFFER_H_
#define _AUDIO_TOOLBOX_CIRCULAR_BUFFER_H_

#include <array>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"

#define _CIRCULAR_BUFFER_TYPE CircularBuffer<SIZE, T>
#define _CIRCULAR_BUFFER_TEMPLATE template <uint32_t SIZE, typename T>

namespace audio_toolbox {

template <uint32_t SIZE, typename T = real_type>
class CircularBuffer {
  static_assert(is_power_of_two(SIZE), "SIZE must be a power of two");

  std::array<T, SIZE> buffer;
  uint32_t tail_index = 0;
  uint32_t head_index = 0;
  uint32_t mask = SIZE - 1;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(CircularBuffer);

  CircularBuffer();

  /**
   * @brief Get a specific element from the circular buffer
   * starting from the head. 0 is the most recent element.
   *
   * @note Calling the operator[] when the buffer is empty is undefined.
   *
   * @param index Relative index from the head.
   * @return T The element at the specified index.
   */
  T operator[](int32_t index);

  /**
   * @brief Get the size of the circular buffer.
   *
   * @return uint32_t The size of the circular buffer.
   */
  uint32_t size() const;

  /**
   * @brief Reset the circular buffer.
   */
  void clear();

  void fill(T value);

  /**
   * @brief Push a new element into the circular buffer.
   *
   * @param data_in New element to push into the circular buffer.
   */
  void push(T data_in);

  /**
   * @brief Pop the oldest element from the circular buffer.
   *
   * @return T The oldest element in the circular buffer.
   */
  T pop();

 private:
  /**
   * @brief Increment the head index.
   */
  inline void increment_head() noexcept;

  /**
   * @brief Increment the tail index.
   */
  inline void increment_tail() noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/core/circular_buffer_imp.hpp"

#endif  // _AUDIO_TOOLBOX_CIRCULAR_BUFFER_H_