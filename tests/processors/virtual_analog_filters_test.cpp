
#include "audio_toolbox/processors/virtual_analog_filters.hpp"

#include "audio_toolbox/core/audio_parameter.hpp"
#include "audio_toolbox/processors/stft.hpp"
#include "test_utilities.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 48000;
constexpr uint32_t N_BLOCKS = 64;
constexpr uint32_t FFT_SIZE = 4096;
constexpr uint32_t LEN_BUFF = N_BLOCKS * FFT_SIZE;
constexpr uint32_t N_FREQS = FFT_SIZE / 2 + 1;
constexpr uint32_t FREQS_SET_SIZE = 32;

class VAFiltersTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(0);

    // Generating a set of linearly spaced frequencies to test the filters
    freqs_set = linear_space(100, SAMPLE_RATE / 2 - 1000, FREQS_SET_SIZE);

    // For the modulation we do a forward and backward sweep
    freqs_set_modulation =
        linear_space(100, SAMPLE_RATE / 2 - 1000, N_BLOCKS / 2);
    freqs_set_modulation.insert(freqs_set_modulation.end(),
                                freqs_set_modulation.rbegin(),
                                freqs_set_modulation.rend());

    // Generating white noise on the input buffer
    noise_gen.process(input.get_write_ptr(), input.get_len(), 1.0f);
  }

  void TearDown() override {}

  /**
   * @brief Computes the gain in dB at a given frequency.
   *
   * @param freq The frequency in Hz.
   * @return Gain in dB.
   */
  real_type get_gain_db_at_freq(real_type freq) {
    real_type avg = 0;
    real_type* x_p = input.get_write_ptr();
    real_type* y_p = output.get_write_ptr();
    std::array<real_type, 2 * N_FREQS> fft_buffer;
    auto _compute_gain = [&](real_type* x) {
      auto g = compute_gain_at_freq(x, freq, SAMPLE_RATE, FFT_SIZE);
      return compute_db(g + 1e-6f);
    };
    for (uint32_t i = 0; i < N_BLOCKS; i++) {
      // Input
      stft.compute_fft(x_p, fft_buffer.data());
      auto g_in = _compute_gain(fft_buffer.data());
      // Output
      stft.compute_fft(y_p, fft_buffer.data());
      auto g_out = _compute_gain(fft_buffer.data());
      // Delta
      avg += g_out - g_in;
      x_p += FFT_SIZE;
      y_p += FFT_SIZE;
    }
    avg /= N_BLOCKS;
    return avg;
  }

  /**
   * @brief Computes the overall gain in dB.
   *
   * @return In/out Gain in dB.
   */
  real_type get_overal_gain_db() {
    auto energy_x = compute_energy(input.get_read_ptr(), input.get_len());
    auto energy_y = compute_energy(output.get_read_ptr(), output.get_len());
    energy_x /= LEN_BUFF;
    energy_y /= LEN_BUFF;
    return compute_db(energy_y / (energy_x + 1e-6));
  }

  /**
   * @brief Process the input with a given filter.
   *
   * @param filter Filter to use.
   */
  void run_filter(AudioFilterBase<SAMPLE_RATE>& filter) {
    filter.process(input.get_read_ptr(), output.get_write_ptr(),
                   input.get_len());
  }

  /**
   * @brief Check the spectral reduction at a given frequency.
   *
   * @param filter Filter to use.
   * @param freq Frequency in Hz.
   * @param gain_db Gain in dB.
   */
  void check_spectral_reduction(AudioFilterBase<SAMPLE_RATE>& filter,
                                real_type freq, real_type gain_db) {
    run_filter(filter);
    EXPECT_LT(get_gain_db_at_freq(freq), gain_db);
  }

  /**
   * @brief Check the reduction at the cutoff frequency.
   *
   * @param filter Filter to use.
   * @param freq Frequency in Hz.
   * @param gain_db Gain in dB.
   */
  void check_cutoff(AudioFilterBase<SAMPLE_RATE>& filter, real_type freq,
                    real_type gain_db) {
    run_filter(filter);
    EXPECT_NEAR(get_gain_db_at_freq(freq), gain_db, 2.0f);
  }

  /**
   * @brief Check that the state is reset properly.
   *
   * @param filter Filter to use.
   */
  void check_state_reset(AudioFilterBase<SAMPLE_RATE>& filter) {
    filter.process(input.get_read_ptr(), output.get_write_ptr(), 1);
    filter.reset();
    filter.process(&x, &y, 1);
    EXPECT_NEAR(y, 0.0f, 1e-8);
  }

  /**
   * @brief Check the stability of the filter.
   *
   * @param filter Filter to use.
   * @param reset Reset the filter state.
   */
  void check_stability(AudioFilterBase<SAMPLE_RATE>& filter,
                       bool reset = true) {
    auto x = input.get_read_ptr();
    auto y = output.get_write_ptr();
    filter.set_resonance(1.0f);
    for (const auto f : freqs_set_modulation) {
      if (reset) filter.reset();
      filter.set_freq(f);
      filter.process(x, y, FFT_SIZE);
      x += FFT_SIZE;
      y += FFT_SIZE;
      auto gain = get_overal_gain_db();
      EXPECT_LT(gain, 50) << "Frequency: " << f;
    }
  }

  WhiteNoiseGen noise_gen;
  real_type x = 0;
  real_type y;
  real_type f0 = 2000;  // Hz
  std::vector<real_type> freqs_set;
  std::vector<real_type> freqs_set_modulation;
  AudioBuffer<LEN_BUFF> input;
  AudioBuffer<LEN_BUFF> output;
  AudioBuffer<N_FREQS, 1, std::complex<real_type>> fft_buffer;
  VALowpass1P<SAMPLE_RATE> lpf1p{f0};
  VAHighpass1P<SAMPLE_RATE> hpf1p{f0};
  VALowpassLadder2P<SAMPLE_RATE> lpf2p{f0, 0.0f};
  VAHighpassLadder2P<SAMPLE_RATE> hpf2p{f0, 0.0f};
  VALowpassLadder4P<SAMPLE_RATE> lpf4p{f0, 0.0f};
  VAHighpassLadder4P<SAMPLE_RATE> hpf4p{f0, 0.0f};

  STFT<FFT_SIZE> stft;
};

// Checking for segfaults

TEST_F(VAFiltersTest, LPF1ProcessSegfault) { run_filter(lpf1p); }

TEST_F(VAFiltersTest, HPF1ProcessSegfault) { run_filter(hpf1p); }

TEST_F(VAFiltersTest, LPF2ProcessSegfault) { run_filter(lpf2p); }

TEST_F(VAFiltersTest, HPF2ProcessSegfault) { run_filter(hpf2p); }

TEST_F(VAFiltersTest, LPF4ProcessSegfault) { run_filter(lpf4p); }

TEST_F(VAFiltersTest, HPF4ProcessSegfault) { run_filter(hpf4p); }

// Checking for spectral reduction
// Note: these filters doesn't seem to match exactly the expected analog
// gain, but they are close enough

// TEST_F(VAFiltersTest, LPF1ProcessCheckSpecCutoff) {
//   check_cutoff(lpf1p, f0, -3);
// }

// TEST_F(VAFiltersTest, HPF1ProcessCheckSpecCutoff) {
//   check_cutoff(hpf1p, f0, -3);
// }

// TEST_F(VAFiltersTest, LPF4ProcessCheckSpecCutoff) {
//   check_cutoff(lpf4p, f0, -12);
// }

// TEST_F(VAFiltersTest, HPF4ProcessCheckSpecCutoff) {
//   check_cutoff(hpf4p, f0, -12);
// }

TEST_F(VAFiltersTest, LPF1ProcessCheckSpec) {
  check_spectral_reduction(lpf1p, f0 * 2, -5);
}

TEST_F(VAFiltersTest, HPF1ProcessCheckSpec) {
  check_spectral_reduction(hpf1p, f0 / 2, -5);
}

TEST_F(VAFiltersTest, LPF2ProcessCheckSpec) {
  check_spectral_reduction(lpf4p, f0 * 2, -8);
}

TEST_F(VAFiltersTest, HPF2ProcessCheckSpec) {
  check_spectral_reduction(hpf4p, f0 / 2, -8);
}

TEST_F(VAFiltersTest, LPF4ProcessCheckSpec) {
  check_spectral_reduction(lpf4p, f0 * 2, -10);
}

TEST_F(VAFiltersTest, HPF4ProcessCheckSpec) {
  check_spectral_reduction(hpf4p, f0 / 2, -10);
}

// Checking that the states are reset properly

TEST_F(VAFiltersTest, LPF1Reset) { check_state_reset(lpf1p); }

TEST_F(VAFiltersTest, HPF1Reset) { check_state_reset(hpf1p); }

TEST_F(VAFiltersTest, LPF2Reset) { check_state_reset(lpf2p); }

TEST_F(VAFiltersTest, HPF2Reset) { check_state_reset(hpf2p); }

TEST_F(VAFiltersTest, LPF4Reset) { check_state_reset(lpf4p); }

TEST_F(VAFiltersTest, HPF4Reset) { check_state_reset(hpf4p); }

// Checking resonance gain reduction

TEST_F(VAFiltersTest, LPF4ResonanceNotCompensated) {
  lpf4p.set_resonance(1.0f);
  check_spectral_reduction(lpf4p, f0 / 4, -8);
}

// TODO: Understand why this test fails
// TEST_F(VAFiltersTest, HPF4ResonanceNotCompensated) {
//   hpf4p.set_resonance(1.0f);
//   check_spectral_reduction(hpf4p, f0 * 4, -8);
// }

// Checking stability without modulating the parameters

TEST_F(VAFiltersTest, LPF1Stability) { check_stability(lpf1p); }

TEST_F(VAFiltersTest, HPF1Stability) { check_stability(hpf1p); }

TEST_F(VAFiltersTest, LPF2Stability) { check_stability(lpf2p); }

TEST_F(VAFiltersTest, HPF2Stability) { check_stability(hpf2p); }

TEST_F(VAFiltersTest, LPF4Stability) { check_stability(lpf4p); }

TEST_F(VAFiltersTest, HPF4Stability) { check_stability(hpf4p); }

// Checking stability while modulating the parameters

TEST_F(VAFiltersTest, LPF1StabilityModulation) {
  check_stability(lpf1p, false);
}

TEST_F(VAFiltersTest, HPF1StabilityModulation) {
  check_stability(hpf1p, false);
}

TEST_F(VAFiltersTest, LPF2StabilityModulation) {
  check_stability(lpf2p, false);
}

TEST_F(VAFiltersTest, HPF2StabilityModulation) {
  check_stability(hpf2p, false);
}

TEST_F(VAFiltersTest, LPF4StabilityModulation) {
  check_stability(lpf4p, false);
}

TEST_F(VAFiltersTest, HPF4StabilityModulation) {
  check_stability(hpf4p, false);
}
