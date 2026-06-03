#include "audio_toolbox/processors/filters/virtual_analog_filter_ladder.hpp"

using namespace audio_toolbox;

VirtualAnalogFilterLadder::VirtualAnalogFilterLadder(uint32_t sample_rate,
                                                     real_type frequency,
                                                     real_type resonance,
                                                     uint32_t num_poles,
                                                     FilterType filter_type)
    : BaseFilter(sample_rate, frequency, resonance),
      one_pole_filters(num_poles),
      gain_powers(num_poles),
      num_poles(num_poles)
{
    if (num_poles < 2)
    {
        throw std::invalid_argument("VirtualAnalogFilterLadder requires at least 2 poles");
    }

    for (uint32_t i = 0; i < num_poles; i++)
    {
        one_pole_filters[i] = std::make_unique<VirtualAnalogFilter1P>(sample_rate, frequency, filter_type);
    }
}

void VirtualAnalogFilterLadder::set_frequency(real_type cutoff_freq) noexcept
{
    for (auto& filter : one_pole_filters)
    {
        filter->set_frequency(cutoff_freq);
    }
    real_type gain_0 = one_pole_filters[0]->get_instantaneuous_gain();
    filter_series_compute_gain_powers(gain_powers, gain_0);
}

real_type VirtualAnalogFilterLadder::filter_sample(real_type input) noexcept
{
    real_type k = get_resonance() * MAX_RESONANCE;

    // Compute feedback sum: k * sum(g^(n-i) * s_i) for i=1..n
    real_type feedback_sum = 0;
    for (uint32_t i = 0; i < num_poles - 1; i++)
    {
        real_type g_power = gain_powers[num_poles - 1 - i];           // g^(n-i)
        real_type s = one_pole_filters[i]->get_instantaneuous_bias(); // s_i
        feedback_sum += g_power * s;
    }
    real_type s_n = one_pole_filters[num_poles - 1]->get_instantaneuous_bias();
    real_type g_n = gain_powers[num_poles - 1];

    // Compute series

    real_type u = (input - k * feedback_sum + s_n) / (1 + g_n * k);

    for (auto& filter : one_pole_filters)
    {
        u = filter->filter_sample(u);
    }

    return u;
}
