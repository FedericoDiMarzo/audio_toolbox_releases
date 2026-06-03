#pragma once
#include "audio_toolbox/processors/filters/virtual_analog_filter_one_pole.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <map>
#include <vector>

namespace audio_toolbox::test
{

struct VirtualAnalogFilter1PFixture
{
    std::vector<ProcessorInstanceFixture<VirtualAnalogFilter1P>> create_configurations() const
    {
        const uint32_t sample_rate = 44100;
        const real_type frequency = 1000.0f;

        std::vector<ProcessorInstanceFixture<VirtualAnalogFilter1P>> configs;
        std::map<BaseFilter::FilterType, std::string> filter_type_to_name = {
            {  BaseFilter::FilterType::LowPass, "LPF1P" },
            { BaseFilter::FilterType::HighPass, "HPF1P" }
        };

        for (auto [filter_type, name] : filter_type_to_name)
        {
            configs.push_back({ .name = name,
                                .factory = [=]()
                                {
                                    return VirtualAnalogFilter1P(sample_rate, frequency, filter_type);
                                } });
        }
        return configs;
    }
};

} // namespace audio_toolbox::test