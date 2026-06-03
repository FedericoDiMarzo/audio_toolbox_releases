#pragma once

#include <stdexcept>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/constexpr.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

#define _CIRCULAR_BUFFER_TYPE     CircularBuffer<T>
#define _CIRCULAR_BUFFER_TEMPLATE template <typename T>

namespace audio_toolbox
{

/**
 * @brief Circular buffer implementation that uses a vector as the underlying
 * storage.
 *
 * @tparam T Type of the elements stored in the circular buffer.
 */
template <typename T>
class CircularBuffer
{
    std::vector<T> buffer;
    uint32_t tail_index;
    uint32_t head_index;
    uint32_t mask;

  public:
    AT_DISABLE_COPY_M(CircularBuffer);

    /**
     * @param length Length of the circular buffer. Must be a power of two.
     */
    CircularBuffer(uint32_t length);

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

    /**
     * @brief Fill the circular buffer with a specific value.
     *
     * @param value The value to fill the circular buffer with.
     */
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

} // namespace audio_toolbox

#include "audio_toolbox/core/circular_buffer_imp.hpp"
