#pragma once
#include "audio_toolbox/processors/oscillators/wavetable_oscillator.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <numbers>
#include <vector>

namespace audio_toolbox::test
{

struct WavetableOscillatorFixture
{
    std::vector<ProcessorInstanceFixture<WavetableOscillator>> create_configurations() const
    {
        const uint32_t sample_rate = 16000;
        const float frequency = 440.0f;


        std::vector<ProcessorInstanceFixture<WavetableOscillator>> configs;
        configs.push_back({ .name = "WavetableOscillator",
                            .factory = [=]()
                            {
                                LookupTable lut = LookupTable([](float x) { return std::sin(x); },
                                                              0.0f,
                                                              2.0f * std::numbers::pi_v<real_type>,
                                                              1024);
                                return WavetableOscillator(std::move(lut), sample_rate, frequency);
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test