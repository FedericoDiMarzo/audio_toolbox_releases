#pragma once
#include "audio_toolbox/processors/filters/biquad.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include <vector>

namespace audio_toolbox::test
{

struct BiquadFixture
{
    std::vector<ProcessorInstanceFixture<Biquad>> create_configurations() const
    {
        const uint32_t sample_rate = 16000;
        const real_type freq = 1000.0f;
        const real_type filter_q = 0.707f;

        std::vector<ProcessorInstanceFixture<Biquad>> configs;
        std::vector<std::string> names = { "Lowpass",  "Highpass", "Lowshelf", "Highshelf",
                                           "Bandpass", "Notch",    "Allpass",  "Peak" };
        // We add all the filter configurations here
        for (size_t i = 0; i < names.size(); ++i)
        {
            BiquadFilterType type = static_cast<BiquadFilterType>(i);
            configs.push_back({ .name = names[i],
                                .factory = [=]()
                                {
                                    return Biquad(type, sample_rate, freq, filter_q);
                                } });
        }
        return configs;
    }
};

} // namespace audio_toolbox::test