#include "audio_toolbox/core/audio_interface.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

// The static flag is used to check if the callback is running.
static unsigned int buffer_size_callback = 0;

void callback([[maybe_unused]] const real_type* input, real_type* output, unsigned int n_samples)
{
    std::fill(output, output + 2 * n_samples, 0.0f);
    buffer_size_callback = n_samples;
}

class AudioInterfaceTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        buffer_size_callback = 0;
    }

    void TearDown() override
    {
        buffer_size_callback = 0;
    }

    unsigned int sample_rate = 16000;
    unsigned int in_channels = 1;
    unsigned int out_channels = 2;
    unsigned int buffer_size = 1024;
    AudioInterfaceMode mode = AudioInterfaceMode::Duplex;
    AudioInterfaceDataType dtype = AudioInterfaceDataType::float32;
    AudioInterface audio_interface{ sample_rate, in_channels, out_channels, buffer_size, dtype, mode, callback };
};

TEST_F(AudioInterfaceTest, Start)
{
    audio_interface.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(buffer_size_callback, buffer_size);
}