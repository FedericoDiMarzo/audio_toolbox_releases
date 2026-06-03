#pragma once
#include "audio_toolbox/processors/YOURPROCESSOR.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct YOURPROCESSORFixture
{
    std::vector<ProcessorInstanceFixture<YOURPROCESSOR>> create_configurations() const
    {
        std::vector<ProcessorInstanceFixture<YOURPROCESSOR>> configs;
        configs.push_back({ .name = "YOURPROCESSOR",
                            .factory = []()
                            {
                                return YOURPROCESSOR();
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test