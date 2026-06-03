#pragma once
#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/random.hpp"
#include "processors_fixtures/processor_instance_fixture.hpp"
#include "test_utilities.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

namespace audio_toolbox::test
{

// Check out ProcessorInstanceFixture for the expected interface of the fixture type T.
template <typename T>
class ProcessorSuiteTest : public ::testing::Test
{
  protected:
    T fixture{};

    // We use this structure to precompute the outputs
    struct ProcessorInstanceResult
    {
        std::string name;
        AudioBuffer input;
        AudioBuffer output;
        uint32_t in_channels;
        uint32_t out_channels;
    };

    std::vector<ProcessorInstanceResult> results;

    void SetUp() override
    {
        for (auto& cfg : fixture.create_configurations())
        {
            auto proc = cfg.factory();

            const uint32_t ic = proc.get_num_in_channels();
            const uint32_t oc = proc.get_num_out_channels();
            const uint32_t num_samples = 16000;
            AudioBuffer input(num_samples, ic);
            AudioBuffer output(num_samples, oc);

            WhiteNoiseGen noise_gen;
            noise_gen.process(input.get_write_ptr(), input.size());

            proc.process(input.get_read_ptr(), output.get_write_ptr(), num_samples);

            results.push_back({ .name = cfg.name,
                                .input = std::move(input),
                                .output = std::move(output),
                                .in_channels = ic,
                                .out_channels = oc });
        }
    }
};

TYPED_TEST_SUITE_P(ProcessorSuiteTest);

TYPED_TEST_P(ProcessorSuiteTest, Instantiation)
{
    for (auto& cfg : this->fixture.create_configurations())
    {
        SCOPED_TRACE("config: " + cfg.name);
        GTEST_LOG_(INFO) << "Configuration: " << cfg.name;
        EXPECT_NO_THROW({ [[maybe_unused]] auto proc = cfg.factory(); });
    }
}

TYPED_TEST_P(ProcessorSuiteTest, NonZeroOutput)
{
    for (auto& r : this->results)
    {
        SCOPED_TRACE("config: " + r.name);
        GTEST_LOG_(INFO) << "Configuration: " << r.name;

        const real_type* out = r.output.get_read_ptr();

        // Count the non-zero samples in the output
        uint32_t nonzero_count =
            std::count_if(out, out + r.output.size(), [](real_type sample) { return sample != 0.0f; });
        float nonzero_ratio = static_cast<float>(nonzero_count) / r.output.size();
        EXPECT_GT(nonzero_ratio, 0.01f) << "Output is mostly zero (" << nonzero_ratio * 100.0f
                                        << "% non-zero samples).";
    }
}

TYPED_TEST_P(ProcessorSuiteTest, Stability)
{
    for (auto& r : this->results)
    {
        SCOPED_TRACE("config: " + r.name);
        GTEST_LOG_(INFO) << "Configuration: " << r.name;

        const real_type* out = r.output.get_read_ptr();

        for (uint32_t i = 0; i < r.output.size(); ++i)
        {
            EXPECT_TRUE(std::isfinite(out[i]))
                << "Non-finite value at sample " << i << " (channel " << (i % r.out_channels) << ").";
        }
    }
}

REGISTER_TYPED_TEST_SUITE_P(ProcessorSuiteTest, Instantiation, NonZeroOutput, Stability);

} // namespace audio_toolbox::test