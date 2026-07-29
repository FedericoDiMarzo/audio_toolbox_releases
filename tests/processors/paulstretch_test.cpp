#include "audio_toolbox/processors/spectral/paulstretch.hpp"

#include <array>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "test_utilities.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 16000;
constexpr uint32_t BUFFER_SIZE = next_power_of_two(SAMPLE_RATE * 3);

uint8_t external_memory[BUFFER_SIZE * sizeof(real_type)];

class PaulStretchTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        set_random_seed(1);
        auto* x = input_buffer.get_write_ptr();
        noise_gen.process(x, input_buffer.size());
    }

    void process_input(PaulStretch& ts)
    {
        ts.process(input_buffer.get_read_ptr(), output_buffer.get_write_ptr(), input_buffer.size());
    }

    void TearDown() override {}

    PaulStretch time_stretch{
        SAMPLE_RATE,
        { 1024, 2048 },
        BUFFER_SIZE
    };
    AudioBuffer input_buffer{ BUFFER_SIZE };
    AudioBuffer output_buffer{ BUFFER_SIZE };
    WhiteNoiseGen noise_gen;
};

TEST_F(PaulStretchTest, ChangeFFTSize)
{
    // Recording
    process_input(time_stretch);
    // First FFT size
    time_stretch.set_recording(false);
    time_stretch.set_seek_position(0.0f);
    process_input(time_stretch);
    for (uint32_t i = 0; i < output_buffer.size(); i++)
    {
        EXPECT_LT(output_buffer[i], 1e8) << "i = " << i;
    }
    // Second FFT size
    time_stretch.set_fft_index(1);
    time_stretch.set_seek_position(0.0f);
    process_input(time_stretch);
    for (uint32_t i = 0; i < output_buffer.size(); i++)
    {
        EXPECT_LT(output_buffer[i], 1e8) << "i = " << i;
    }
}