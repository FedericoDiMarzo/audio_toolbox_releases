#pragma once
#include "audio_toolbox/processors/filters/virtual_analog_filter_ladder.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <map>
#include <vector>

namespace audio_toolbox::test
{

struct VirtualAnalogFilterLadderFixture
{
    std::vector<ProcessorInstanceFixture<VirtualAnalogFilterLadder>> create_configurations() const
    {
        const uint32_t sample_rate = 44100;
        const real_type frequency = 1000.0f;
        const real_type resonance = 0.5f;
        std::vector<uint32_t> num_poles_options = { 2, 4 };

        std::vector<ProcessorInstanceFixture<VirtualAnalogFilterLadder>> configs;
        std::map<BaseFilter::FilterType, std::string> filter_type_to_name = {
            {  BaseFilter::FilterType::LowPass, "LPF" },
            { BaseFilter::FilterType::HighPass, "HPF" }
        };

        for (auto [filter_type, name] : filter_type_to_name)
        {
            for (uint32_t num_poles : num_poles_options)
            {
                configs.push_back(
                    { .name = name + std::to_string(num_poles) + "P",
                      .factory = [=]()
                      {
                          return VirtualAnalogFilterLadder(sample_rate, frequency, resonance, num_poles, filter_type);
                      } });
            }
        }
        return configs;
    }
};

} // namespace audio_toolbox::test