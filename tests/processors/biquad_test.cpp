#include "audio_toolbox/processors/filters/biquad.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

using BFT = BiquadFilterType;

class BiquadTest : public ::testing::Test
{
  protected:
    void SetUp() override {}

    void TearDown() override {}

    unsigned int sample_rate = 16000;
    real_type freq = 1000.0f;
    real_type q = 0.707f;
    real_type gain_db = 0.0f;
    Biquad lpf{ BFT::Lowpass, sample_rate, freq, q };
    Biquad hpf{ BFT::Highpass, sample_rate, freq, q };
    Biquad lsf{ BFT::Lowshelf, sample_rate, freq, q, gain_db };
    Biquad hsf{ BFT::Highshelf, sample_rate, freq, q, gain_db };
    Biquad bpf{ BFT::Bandpass, sample_rate, freq, q };
    Biquad notch{ BFT::Notch, sample_rate, freq, q };
    Biquad allpass{ BFT::Allpass, sample_rate, freq, q };
    Biquad peak{ BFT::Peak, sample_rate, freq, q, gain_db };
};

TEST_F(BiquadTest, IsStable)
{
    EXPECT_TRUE(lpf.is_stable());
    EXPECT_TRUE(hpf.is_stable());
    EXPECT_TRUE(lsf.is_stable());
    EXPECT_TRUE(hsf.is_stable());
    EXPECT_TRUE(bpf.is_stable());
    EXPECT_TRUE(notch.is_stable());
    EXPECT_TRUE(allpass.is_stable());
    EXPECT_TRUE(peak.is_stable());
}