#pragma once

#include <cstdint>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{


/**
 * @brief Lookup table for approximating functions.
 */
class LookupTable
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==
    real_type arg_min;
    real_type arg_max;
    real_type arg_range;
    std::vector<real_type> table;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    LookupTable(LookupTable&&) = default;
    LookupTable& operator=(LookupTable&&) = default;
    LookupTable(const LookupTable&) = default;
    LookupTable& operator=(const LookupTable&) = default;

    /**
     * @param function Function to store in the table.
     * @param arg_min Minimum argument of the function.
     * @param arg_max Maximum argument of the function.
     * @param lut_length Length of the lookup table.
     */
    LookupTable(std::function<float(float)> function, real_type arg_min, real_type arg_max, uint32_t lut_length);

    /**
     * @param lut_table Precomputed lookup table.
     * @param arg_min Minimum argument of the function.
     * @param arg_max Maximum argument of the function.
     */
    LookupTable(std::vector<real_type> lut_table, real_type arg_min, real_type arg_max);


    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    real_type operator()(real_type x) const noexcept;

    inline size_t size() const noexcept
    {
        return table.size();
    }

    inline real_type get_arg_min() const noexcept
    {
        return arg_min;
    }

    inline real_type get_arg_max() const noexcept
    {
        return arg_max;
    }

    inline real_type get_arg_range() const noexcept
    {
        return arg_range;
    }
};

} // namespace audio_toolbox