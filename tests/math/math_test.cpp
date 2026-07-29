
#include "audio_toolbox/processors/spectral/stft.hpp"
#include "audio_toolbox/static/audio_buffer_static.hpp"
#include "test_utilities.hpp"

using namespace audio_toolbox;

const uint32_t N_FFT = 64;
const uint32_t N_FREQS = N_FFT / 2 + 1;
const uint32_t SAMPLE_RATE = 16000;

class MathTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        set_random_seed(0);
        white_noise_gen.process(input.get_write_ptr(), N_FFT, 1.0f);
        fft.process(input.get_read_ptr(), input_fft.get_write_ptr());
    }

    void TearDown() override {}

    WhiteNoiseGen white_noise_gen;
    AudioBufferStatic<N_FFT> input;
    AudioBufferStatic<2 * N_FREQS> input_fft;
    STFT stft{ N_FFT, N_FFT };
    FFT& fft = stft.get_fft();
};

TEST_F(MathTest, InvertDB)
{
    real_type x = 132.523f;
    auto y = compute_db(x);
    auto z = compute_inverse_db(y);
    EXPECT_NEAR(x, z, 1e-4);
}

TEST_F(MathTest, Energy)
{
    std::vector<real_type> x = { 1.0f, 2.0f, -3.0f };
    auto energy = compute_energy(x.data(), x.size());
    EXPECT_NEAR(energy, 14.0f, 1e-8);
}

TEST_F(MathTest, Avg)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 3.0f };
    auto avg = compute_avg(x.data(), x.size());
    EXPECT_NEAR(avg, 0.0f, 1e-8);
}

TEST_F(MathTest, Std)
{
#if AT_ENABLE_APPROX_MATH == 1
    real_type eps = 0.1;
#else
    real_type eps = 1e-8;
#endif
    std::vector<real_type> x = { 0.0f, -3.0f, 3.0f };
    auto std = compute_std(x.data(), x.size());
    auto energy = compute_energy(x.data(), x.size());
    EXPECT_NEAR(std, std::sqrt(energy / x.size()), eps);
}

TEST_F(MathTest, MSE)
{
    std::vector<real_type> x = { 0.0f, -2.0f, 2.0f };
    std::vector<real_type> y = { 0.0f, 0.0f, 0.0f };
    auto mse = compute_mse(x.data(), y.data(), x.size());
    EXPECT_NEAR(mse, 8.0f / 3.0f, 1e-8);
}

TEST_F(MathTest, MAE)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 3.0f };
    std::vector<real_type> y = { 0.0f, 3.0f, -3.0f };
    auto mae = compute_mae(x.data(), y.data(), x.size());
    EXPECT_NEAR(mae, 4.0f, 1e-8);
}

TEST_F(MathTest, ApplyGain)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 3.0f };
    auto gain = 2.0f;
    apply_gain(x.data(), x.size(), gain);
    EXPECT_NEAR(x[0], 0.0f, 1e-8);
    EXPECT_NEAR(x[1], -6.0f, 1e-8);
    EXPECT_NEAR(x[2], 6.0f, 1e-8);
}

TEST_F(MathTest, ApplyGainDB)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 3.0f };
    auto gain_db = 6.0f;
    apply_gain_db(x.data(), x.size(), gain_db);
    EXPECT_NEAR(x[0], 0.0f, 1e-1);
    EXPECT_NEAR(x[1], -6.0f, 1e-1);
    EXPECT_NEAR(x[2], 6.0f, 1e-1);
}

TEST_F(MathTest, LinearInterpolation)
{
    auto y = linear_interpolation(0.0f, 2.2f, 0.5f);
    EXPECT_NEAR(y, 1.1f, 1e-8);
}

TEST_F(MathTest, ClipMax)
{
    auto y1 = clip_max(0.5f, 1.0f);
    EXPECT_NEAR(y1, 0.5f, 1e-8);
    auto y2 = clip_max(1.5f, 1.0f);
    EXPECT_NEAR(y2, 1.0f, 1e-8);
}

TEST_F(MathTest, ClipMin)
{
    auto y1 = clip_min(0.5f, 1.0f);
    EXPECT_NEAR(y1, 1.0f, 1e-8);
    auto y2 = clip_min(1.5f, 1.0f);
    EXPECT_NEAR(y2, 1.5f, 1e-8);
}

TEST_F(MathTest, Clip)
{
    auto y1 = clip(0.5f, 0.0f, 2.0f);
    EXPECT_NEAR(y1, 0.5f, 1e-8);
    auto y2 = clip(0.2f, 1.0f, 2.0f);
    EXPECT_NEAR(y2, 1.0f, 1e-8);
    auto y3 = clip(2.5f, 1.0f, 2.0f);
    EXPECT_NEAR(y3, 2.0f, 1e-8);
}

TEST_F(MathTest, ComputeBinFromFreq)
{
    auto bin = compute_bin_from_freq(SAMPLE_RATE / 4, SAMPLE_RATE, N_FFT);
    EXPECT_EQ(bin, N_FREQS / 2);
}

TEST_F(MathTest, LinearMap)
{
    auto x = linear_map(0.4, 0.0, 1.0, 0.0, 10.0);
    EXPECT_NEAR(x, 4.0, 1e-8);
}

TEST_F(MathTest, AlphaFromTaoBoundaries)
{
    auto x = alpha_from_time_const(0.1, 0.2);
    auto y = alpha_from_time_const(-0.2, 0.1);
    EXPECT_NEAR(x, 1.0, 1e-8);
    EXPECT_NEAR(y, 0.0, 1e-8);
}

TEST_F(MathTest, EMAConvergence)
{
    const unsigned int len = 2000;
    auto x = get_zeros<real_type, len>();
    auto y = get_zeros<real_type, len>();
    x[0] = 1.0f;
    compute_ema(x.data(), y.data(), len, 0.9);
    compute_abs(y.data(), y.data(), len);
    real_type y_max = compute_max(y.data(), len);
    EXPECT_LT(y_max, 1.0);
}

TEST_F(MathTest, EMADivergence)
{
    const unsigned int len = 2000;
    auto x = get_zeros<real_type, len>();
    auto y = get_zeros<real_type, len>();
    x[0] = 1.0f;
    compute_ema(x.data(), y.data(), len, 1.1);
    compute_abs(y.data(), y.data(), len);
    real_type y_max = compute_max(y.data(), len);
    EXPECT_GT(y_max, 2.0);
}

TEST_F(MathTest, Abs)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 2.0f };
    std::vector<real_type> y_exp = { 0.0f, 3.0f, 2.0f };
    std::vector<real_type> y(x.size());
    compute_abs(x.data(), y.data(), x.size());
    expect_all_near(y.data(), y_exp.data(), x.size(), 1e-8);
}

TEST_F(MathTest, Max)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 2.0f };
    auto max = compute_max(x.data(), x.size());
    EXPECT_NEAR(max, 2.0f, 1e-8);
}

TEST_F(MathTest, Min)
{
    std::vector<real_type> x = { 0.0f, -3.0f, 2.0f };
    auto min = compute_min(x.data(), x.size());
    EXPECT_NEAR(min, -3.0f, 1e-8);
}

TEST_F(MathTest, PeakNormalization)
{
    std::vector<real_type> x = { 0.0f, -4.0f, 2.0f };
    std::vector<real_type> y = { 0.0f, 0.0f, 0.0f };
    std::vector<real_type> expected = { 0.0f, -1.0f, 0.5f };
    apply_peak_normalization(x.data(), y.data(), x.size());
    expect_all_near(y.data(), expected.data(), x.size());
}

TEST_F(MathTest, IsPowerOfTwo)
{
    EXPECT_TRUE(is_power_of_two(2));
    EXPECT_TRUE(is_power_of_two(64));
    EXPECT_FALSE(is_power_of_two(61));
    EXPECT_FALSE(is_power_of_two(61));
    EXPECT_FALSE(is_power_of_two(6));
}

TEST_F(MathTest, NextPowerOfTwo)
{
    EXPECT_EQ(next_power_of_two(0), 0);
    EXPECT_EQ(next_power_of_two(2), 2);
    EXPECT_EQ(next_power_of_two(3), 4);
    EXPECT_EQ(next_power_of_two(1029), 2048);
}

TEST_F(MathTest, ComputeIntRange)
{
    auto range = create_int_range(20, 30);
    EXPECT_EQ(range.size(), 10);
    EXPECT_EQ(range[0], 20);
    EXPECT_EQ(range[9], 29);
}

TEST_F(MathTest, ExponentialMappingBoundaries)
{
    real_type min = 30.0f;
    real_type max = 20000.0f;
    real_type y0 = exponential_mapping(0.0f, min, max);
    real_type y1 = exponential_mapping(1.0f, min, max);
    EXPECT_NEAR(y0, min, 1e-8);
    EXPECT_NEAR(y1, max, 1e-8);
}
