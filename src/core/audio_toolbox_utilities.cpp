#include "audio_toolbox/static/templates_utilities.hpp"

using namespace audio_toolbox;

std::vector<real_type> audio_toolbox::linear_space(real_type start, real_type end, uint32_t n)
{
    std::vector<real_type> result(n);
    std::iota(result.begin(), result.end(), 0);
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [=](real_type i) { return linear_map(i, 0, n - 1, start, end); });
    return result;
}
