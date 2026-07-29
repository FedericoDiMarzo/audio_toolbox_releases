#pragma once
#include "audio_toolbox/processors/delay/basic_chorus.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct BasicChorusFixture
{
    std::vector<ProcessorInstanceFixture<BasicChorus>> create_configurations() const
    {
        const real_type sample_rate = 16000.0f;
        std::vector<ProcessorInstanceFixture<BasicChorus>> configs;
        configs.push_back({ .name = "BasicChorus",
                            .factory = [=]()
                            {
                                return BasicChorus::create(sample_rate, BasicChorus::Preset::SlowLFOModulator);
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test