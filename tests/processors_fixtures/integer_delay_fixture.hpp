#pragma once
#include "audio_toolbox/processors/delay/integer_delay.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct IntegerDelayFixture
{
    std::vector<ProcessorInstanceFixture<IntegerDelay>> create_configurations() const
    {
        std::vector<ProcessorInstanceFixture<IntegerDelay>> configs;
        configs.push_back({ .name = "IntegerDelay",
                            .factory = [=]()
                            {
                                return IntegerDelay(1000);
                            } });
        return configs;
    }
};

} // namespace audio_toolbox::test