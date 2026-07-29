#pragma once
#include "audio_toolbox/processors/spectral/paulstretch.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct PaulStretchFixture
{
    std::vector<ProcessorInstanceFixture<PaulStretch>> create_configurations() const
    {
        const uint32_t sample_rate = 16000;

        std::vector<ProcessorInstanceFixture<PaulStretch>> configs;
        configs.push_back({ .name = "PaulStretch",
                            .factory = [=]()
                            {
                                return PaulStretch(sample_rate, { 512, 1024, 2048 }

                                );
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test