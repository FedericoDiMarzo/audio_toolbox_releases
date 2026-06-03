#include "audio_toolbox/processors/delay/basic_chorus.hpp"

using namespace audio_toolbox;

namespace BasicChorusPresetsConstruction
{


BasicChorus create_preset_slow_lfo_modulator(uint32_t sample_rate,
                                             uint32_t num_delay_lines,
                                             uint32_t lut_length,
                                             real_type depth,
                                             real_type min_delay,
                                             real_type max_delay,
                                             real_type mix) noexcept
{
    std::vector<std::unique_ptr<FractionalDelay>> delay_lines;
    LookupTable lfo_lut([](real_type x) { return std::sin(x); }, 0.0f, 2.0f * M_PI, lut_length);
    for (uint32_t i = 0; i < num_delay_lines; i++)
    {
        auto lfo = std::make_unique<WavetableOscillator>(lfo_lut, sample_rate, 1.0f);
        auto delay_line = std::make_unique<FractionalDelay>(max_delay, std::move(lfo));
        delay_line->set_random_range(min_delay, max_delay);
        delay_line->set_modulation_frequency(0.1f, 0.5f);
        delay_line->randomize();
        delay_lines.push_back(std::move(delay_line));
    }
    return BasicChorus(delay_lines, depth, mix);
}
} // namespace BasicChorusPresetsConstruction

BasicChorus BasicChorus::create(uint32_t sample_rate,
                                Preset preset,
                                uint32_t num_delay_lines,
                                uint32_t lut_length,
                                real_type depth,
                                real_type min_delay,
                                real_type max_delay,
                                real_type mix) noexcept
{
    switch (preset)
    {
        case Preset::SlowLFOModulator:
        default:
        {
            return BasicChorusPresetsConstruction::create_preset_slow_lfo_modulator(sample_rate,
                                                                                    num_delay_lines,
                                                                                    lut_length,
                                                                                    depth,
                                                                                    min_delay,
                                                                                    max_delay,
                                                                                    mix);
        }
    }
}

BasicChorus::BasicChorus(std::span<std::unique_ptr<FractionalDelay>> delay_lines, real_type depth, real_type mix)
    : mix(clip(mix, 0.0f, 1.0f)),
      depth(depth)
{
    for (auto& dl : delay_lines)
    {
        this->delay_lines.push_back(std::move(dl));
    }
    set_depth(depth);
}

real_type BasicChorus::process_mono_sample(real_type input) noexcept
{
    real_type x = 0.0f;
    for (auto& dl : delay_lines)
    {
        x += dl->process_mono_sample(input);
    }
    return (1.0f - mix) * input + mix * x / delay_lines.size();
}
