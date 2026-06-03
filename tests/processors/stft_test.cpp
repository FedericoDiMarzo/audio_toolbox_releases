#include "audio_toolbox/processors/spectral/stft.hpp"
#include "test_utilities.hpp"

const uint32_t FFT_SIZE = 32;
const uint32_t N_FREQS = FFT_SIZE / 2 + 1;
const uint32_t N_FRAMES = 10;
const uint32_t WIN_SIZE = FFT_SIZE / 2;
const uint32_t HOPSIZE = WIN_SIZE / 2;

using namespace audio_toolbox;

class STFTTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        set_random_seed(0);
        set_random_input();
    }

    void TearDown() override {}

    void set_random_input()
    {
        white_noise.process(input.get_write_ptr(), input.get_num_samples(), 1);
        white_noise.process(input_stft.get_write_ptr(), input_stft.get_num_samples(), 1);
    }

    float eps = 1e-5f;

    AudioBuffer input{ FFT_SIZE };
    AudioBuffer output{ 2 * N_FREQS };
    AudioBuffer tmp_buff{ 2 * N_FREQS };
    AudioBuffer input_stft{ N_FRAMES * HOPSIZE };
    AudioBuffer output_stft{ N_FRAMES * HOPSIZE };
    WhiteNoiseGen white_noise;
    STFT stft{ WIN_SIZE, FFT_SIZE };
    FFT& fft = stft.get_fft();
};

TEST_F(STFTTest, ComputeFFTReordering)
{
    fft.process(input.get_read_ptr(), output.get_write_ptr());
    // DC
    EXPECT_NEAR(output[1], 0.0f, eps);
    // Nyquist
    EXPECT_NEAR(output[FFT_SIZE + 1], 0.0f, eps);
}

TEST_F(STFTTest, TestFFTInversion)
{
    auto x = input.get_read_ptr();
    auto y = output.get_write_ptr();
    auto tmp = tmp_buff.get_write_ptr();
    fft.process(x, tmp);
    fft.process_inverse(tmp, y);
    expect_all_near(x, y, FFT_SIZE, eps);
}

TEST_F(STFTTest, TestSTFTInversion)
{
    auto x = input_stft.get_read_ptr();
    auto y = output_stft.get_read_ptr();
    auto x_p = input_stft.get_read_ptr();
    auto y_p = output_stft.get_write_ptr();
    auto tmp = tmp_buff.get_write_ptr();

    for (unsigned int i = 0; i < N_FRAMES - 1; i++)
    {
        stft.process(x_p, tmp);
        stft.process_inverse(tmp, y_p);
        x_p += HOPSIZE;
        y_p += HOPSIZE;
    }
    expect_all_near(x, y + HOPSIZE, input_stft.get_num_samples() - 4 * HOPSIZE, eps);
}
