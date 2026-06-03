#pragma once

#include <array>
#include <cstdint>

namespace audio_toolbox
{

/**
 * @brief Calculate the maximum value in a std::array at compile time.
 *
 * @tparam T
 * @tparam N
 * @param arr
 * @return constexpr T
 */
template <typename T, size_t N>
constexpr T array_max(const std::array<T, N>& arr)
{
    T max = arr[0];
    for (size_t i = 1; i < N; ++i)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }
    return max;
}

/**
 * @brief Check if a number is a power of two.
 *
 * @param n Number to check.
 * @return True if the number is a power of two, false otherwise.
 */
constexpr bool is_power_of_two(uint32_t n)
{
    return n && ((n & (n - 1)) == 0);
}

/**
 * @brief Compute the next power of two.
 *
 * This function works by first decrementing the input value v, then repeatedly
 * "smearing" the highest set bit to the right until all bits to the right of
 * the highest set bit are also set. Finally, it increments v to reach the next
 * power of two.
 *
 * @param v Input value.
 * @return Next power of two.
 */
constexpr uint32_t next_power_of_two(uint32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

} // namespace audio_toolbox
