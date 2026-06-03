#include "audio_toolbox/processors/oscillators/wavetable_oscillator.hpp"

using namespace audio_toolbox;

WavetableOscillator::WavetableOscillator(LookupTable lut,
                                         uint32_t sample_rate,
                                         real_type frequency,
                                         real_type normalized_phase)
    : BaseOscillator(sample_rate, frequency, normalized_phase),
      lut(lut)
{
    set_frequency(frequency);
    reset();
}

void WavetableOscillator::reset() noexcept
{
    BaseOscillator::reset();
    table_idx = normalized_phase * lut.get_arg_range() + lut.get_arg_min();
}

void WavetableOscillator::set_frequency(real_type frequency) noexcept
{
    BaseOscillator::set_frequency(frequency);
    table_idx_increment = frequency / sample_rate * lut.get_arg_range();
}

real_type WavetableOscillator::generate_sample() noexcept

{
    real_type y = lut(table_idx);
    table_idx += table_idx_increment;
    if (table_idx >= lut.get_arg_max())
    {
        table_idx -= lut.get_arg_range();
    }
    return y;
}

void WavetableOscillator::randomize() noexcept
{
    normalized_phase = get_random_real(0.0f, 1.0f);
}
