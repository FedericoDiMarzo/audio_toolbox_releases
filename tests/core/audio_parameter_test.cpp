#include "audio_toolbox/core/audio_parameter.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class AudioParameterTest : public ::testing::Test
{
  protected:
    void SetUp() override {}

    void TearDown() override {}

    unsigned int sample_rate = 16000;
    unsigned int block_size = 64;
    float transition_ms = 10.0f;
    unsigned int total_updates;
    real_type start_value = 0.0f;
    AudioParameter param{ start_value, sample_rate, block_size, transition_ms };
};

TEST_F(AudioParameterTest, InitialValue)
{
    EXPECT_NEAR(param.get(), start_value, 1e-8);
}

TEST_F(AudioParameterTest, SetValue)
{
    real_type new_value = 0.5f;
    param.set(new_value);
    for (unsigned int i = 0; i < 100; i++)
    {
        param.update();
    }
    EXPECT_NEAR(param.get(), new_value, 1e-6);
}