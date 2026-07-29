#include "audio_toolbox/processors/delay/fractional_delay.hpp"

using namespace audio_toolbox;

FractionalDelay::FractionalDelay(real_type delay, std::unique_ptr<WavetableOscillator> lfo)
    : delay_line(static_cast<uint32_t>(delay) + 1),
      lfo(std::move(lfo)),
      modulation_depth(0.0f)
{
    set_delay(delay);
    set_random_range(0.0f, delay);
    reset();
}

void FractionalDelay::set_modulation_frequency(real_type freq, real_type freq_max) noexcept
{
    if (!lfo)
    {
        return;
    }

    freq = clip(freq, 0.0f, lfo->get_sample_rate() / 2.0f);

    if (freq_max > freq)
    {
        freq = get_random_real(freq, freq_max);
    }

    lfo->set_frequency(freq);
}

real_type FractionalDelay::get_modulation_frequency() const noexcept
{
    if (!lfo)
    {
        return 0.0f;
    }
    return lfo->get_frequency();
}

real_type FractionalDelay::process_mono_sample(

    real_type input) noexcept
{
    real_type delay_delta;
    if (lfo)
    {
        delay_delta = modulation_depth * lfo->generate_sample();
    }
    else
    {
        delay_delta = 0.0f;
    }
    auto& cb = delay_line.get_buffer();
    cb.push(input);
    real_type current_delay = delay + delay_delta;
    uint32_t delay_int = static_cast<uint32_t>(current_delay);
    real_type delay_dec = current_delay - delay_int;
    real_type val1 = cb[delay_int];
    real_type val2 = cb[delay_int + 1];
    return linear_interpolation(val1, val2, delay_dec);
}
