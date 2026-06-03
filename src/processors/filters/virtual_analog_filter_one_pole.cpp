#include "audio_toolbox/processors/filters/virtual_analog_filter_one_pole.hpp"

using namespace audio_toolbox;

VirtualAnalogFilter1P::VirtualAnalogFilter1P(uint32_t sample_rate, real_type frequency, FilterType type)
    : BaseFilter(sample_rate, frequency, 0.0f)
{
    set_frequency(frequency);
    reset();

    specialization.type = type;
    switch (type)
    {
        case FilterType::LowPass:
            specialization.filter_sample_fn = [this](real_type input)
            {
                return filter_sample_lpf(input);
            };
            specialization.get_instantaneuous_gain_fn = [this]()
            {
                return get_instantaneuous_gain_lpf();
            };
            specialization.get_instantaneuous_bias_fn = [this]()
            {
                return get_instantaneuous_bias_lpf();
            };
            break;
        case FilterType::HighPass:
            specialization.filter_sample_fn = [this](real_type input)
            {
                return filter_sample_hpf(input);
            };
            specialization.get_instantaneuous_gain_fn = [this]()
            {
                return get_instantaneuous_gain_hpf();
            };
            specialization.get_instantaneuous_bias_fn = [this]()
            {
                return get_instantaneuous_bias_hpf();
            };
            break;
    }
}

void VirtualAnalogFilter1P::set_frequency(real_type cutoff_freq) noexcept
{
    g = compute_tan(M_PI * cutoff_freq / get_sample_rate());
    G = g / (1.0f + g);
}

real_type VirtualAnalogFilter1P::get_instantaneuous_gain() const noexcept
{
    return specialization.get_instantaneuous_gain_fn();
}

real_type VirtualAnalogFilter1P::get_instantaneuous_bias() const noexcept
{
    return specialization.get_instantaneuous_bias_fn();
}

real_type VirtualAnalogFilter1P::filter_sample(real_type input) noexcept
{
    return specialization.filter_sample_fn(input);
}

// Specialized implementations for LPF and HPF == == == == == == == == == == == == == ==


real_type VirtualAnalogFilter1P::filter_sample_lpf(real_type input) noexcept
{
    real_type v = (input - s) * G;
    real_type y = v + s;
    s = v + y;
    return y;
}

real_type VirtualAnalogFilter1P::filter_sample_hpf(real_type input) noexcept
{
    return input - filter_sample_lpf(input);
}

real_type VirtualAnalogFilter1P::get_instantaneuous_gain_lpf() const noexcept
{
    return G;
}

real_type VirtualAnalogFilter1P::get_instantaneuous_gain_hpf() const noexcept
{
    return G / g;
}

real_type VirtualAnalogFilter1P::get_instantaneuous_bias_lpf() const noexcept
{
    return s / (1 + g);
}

real_type VirtualAnalogFilter1P::get_instantaneuous_bias_hpf() const noexcept
{
    return -get_instantaneuous_bias_lpf();
}