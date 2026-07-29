#pragma once
#include "audio_toolbox/processors/distortions/bitcrush.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct BitCrushFixture
{
    std::vector<ProcessorInstanceFixture<BitCrush>> create_configurations() const
    {
        std::vector<ProcessorInstanceFixture<BitCrush>> configs;
        configs.push_back({ .name = "BitCrush",
                            .factory = []()
                            {
                                return BitCrush();
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test