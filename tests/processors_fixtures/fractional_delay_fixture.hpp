#pragma once
#include "audio_toolbox/processors/delay/fractional_delay.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct FractionalDelayFixture
{
    std::vector<ProcessorInstanceFixture<FractionalDelay>> create_configurations() const
    {
        std::vector<ProcessorInstanceFixture<FractionalDelay>> configs;
        configs.push_back({ .name = "FractionalDelay",
                            .factory = [=]()
                            {
                                return FractionalDelay(1000.0f);
                            } });
        configs.push_back({ .name = "ModulatedFractionalDelay",
                            .factory = [=]()
                            {
                                LookupTable sine_lut([](float x) { return std::sin(x); }, 0.0f, 2.0f * M_PI, 1024);
                                std::unique_ptr<WavetableOscillator> lfo =
                                    std::make_unique<WavetableOscillator>(std::move(sine_lut), 44100, 1000.0f);
                                return FractionalDelay(1000.0f, std::move(lfo));
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test