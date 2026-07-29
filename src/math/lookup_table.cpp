#include "audio_toolbox/math/lookup_table.hpp"

using namespace audio_toolbox;

LookupTable::LookupTable(std::function<float(float)> function,
                         real_type arg_min,
                         real_type arg_max,
                         uint32_t lut_length)
    : arg_min(arg_min),
      arg_max(arg_max),
      arg_range(arg_max - arg_min),
      table(lut_length)
{
    real_type step = (arg_max - arg_min) / (size() - 1);
    for (unsigned int i = 0; i < size(); i++)
    {
        table[i] = function(arg_min + i * step);
    }
}

LookupTable::LookupTable(std::vector<real_type> lut_table, real_type arg_min, real_type arg_max)
    : arg_min(arg_min),
      arg_max(arg_max),
      arg_range(arg_max - arg_min),
      table(std::move(lut_table))
{
}

real_type LookupTable::operator()(real_type x) const noexcept
{
    if (x <= arg_min)
    {
        return table[0];
    }
    else if (x >= arg_max)
    {
        return table[size() - 1];
    }
    else
    {
        real_type idx = (x - arg_min) / arg_range * (size() - 1);
        unsigned int i = static_cast<uint32_t>(idx);
        return linear_interpolation(table[i], table[i + 1], idx - i);
    }
}