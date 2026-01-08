#ifndef AUDIO_TOOLBOX_CONSTEXPR_H_
#define AUDIO_TOOLBOX_CONSTEXPR_H_

#include <array>
#include <cstdint>

namespace audio_toolbox {

/**
 * @brief Calculate the maximum value in a std::array at compile time.
 *
 * @tparam T
 * @tparam N
 * @param arr
 * @return constexpr T
 */
template <typename T, size_t N>
constexpr T array_max(const std::array<T, N>& arr) {
  T max = arr[0];
  for (size_t i = 1; i < N; ++i) {
    if (arr[i] > max) {
      max = arr[i];
    }
  }
  return max;
}

}  // namespace audio_toolbox

#endif  // AUDIO_TOOLBOX_CONSTEXPR_H_