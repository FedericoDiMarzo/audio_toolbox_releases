#ifndef _AUDIO_TOOLBOX_LOOKUP_TABLE_H_
#define _AUDIO_TOOLBOX_LOOKUP_TABLE_H_

#include <array>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"

#define _LOOKUP_TABLE_TYPE LookupTable<SIZE>
#define _LOOKUP_TABLE_TEMPLATE template <uint32_t SIZE>

// TODO: Tests

namespace audio_toolbox {

/**
 * @brief Lookup table for a function.
 *
 * @tparam SIZE Size of the table.
 */
template <uint32_t SIZE>
class LookupTable {
  real_type arg_min;
  real_type arg_max;
  real_type arg_range;
  std::array<real_type, SIZE> table;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(LookupTable);

  /**
   * @brief Construct a new Lookup Table.
   *
   * @param function Function to store in the table.
   * @param arg_min Minimum argument of the function.
   * @param arg_max Maximum argument of the function.
   */
  LookupTable(std::function<float(float)> function, real_type arg_min,
              real_type arg_max);

  /**
   * @brief Get the value of the function at a given point.
   *
   * The value will be approximated as a linear interpolation between the
   * closest points in the table. If the argument is out of the range of the
   * table, one of the extreme values will be returned.
   *
   * @param x Argument of the function.
   * @return Value of the function at x.
   */
  inline real_type operator()(real_type x) const noexcept;

  /**
   * @brief Get the size of the table.
   *
   * @return Size of the table.
   */
  inline size_t size() const noexcept;

  /**
   * @brief Get the minimum argument of the function.
   *
   * @return Minimum argument of the function.
   */
  inline real_type get_arg_min() const noexcept;

  /**
   * @brief Get the maximum argument of the function.
   *
   * @return Maximum argument of the function.
   */
  inline real_type get_arg_max() const noexcept;

  /**
   * @brief Get the range of the argument of the function.
   *
   * @return Range of the argument of the function.
   */
  inline real_type get_arg_range() const noexcept;
};

}  // namespace audio_toolbox

#include "audio_toolbox/math/lookup_table_imp.hpp"

#endif  // _AUDIO_TOOLBOX_LOOKUP_TABLE_H_