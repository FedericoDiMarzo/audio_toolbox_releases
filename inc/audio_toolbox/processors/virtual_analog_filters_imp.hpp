

namespace audio_toolbox {

// VA Lowpass 1P ==============================================================

_VA_LOWPASS_1P_TEMPLATE
_VA_LOWPASS_1P_TYPE::VALowpass1P(real_type cutoff_freq) {
  set_freq(cutoff_freq);
  reset();
}

_VA_LOWPASS_1P_TEMPLATE
void _VA_LOWPASS_1P_TYPE::reset() noexcept { s = 0.0f; }

_VA_LOWPASS_1P_TEMPLATE
real_type _VA_LOWPASS_1P_TYPE::process_sample(real_type input) noexcept {
  real_type v = (input - s) * G;
  real_type output = v + s;
  s = v + output;
  return output;
}

_VA_LOWPASS_1P_TEMPLATE
void _VA_LOWPASS_1P_TYPE::set_freq(real_type cutoff_freq) noexcept {
  g = compute_tan(M_PI * cutoff_freq / SAMPLE_RATE);
  G = g / (1.0f + g);
}

_VA_LOWPASS_1P_TEMPLATE
real_type _VA_LOWPASS_1P_TYPE::get_instantaneuous_gain() const noexcept {
  return G;
}

_VA_LOWPASS_1P_TEMPLATE
real_type _VA_LOWPASS_1P_TYPE::get_instantaneuous_bias() const noexcept {
  return s / (1 + g);
}

// VA Highpass 1P =============================================================

_VA_HIGHPASS_1P_TEMPLATE
_VA_HIGHPASS_1P_TYPE::VAHighpass1P(real_type cutoff_freq)
    : VALowpass1P<SAMPLE_RATE>(cutoff_freq) {}

_VA_HIGHPASS_1P_TEMPLATE
real_type _VA_HIGHPASS_1P_TYPE::process_sample(real_type input) noexcept {
  return input - VALowpass1P<SAMPLE_RATE>::process_sample(input);
}

_VA_HIGHPASS_1P_TEMPLATE
real_type _VA_HIGHPASS_1P_TYPE::get_instantaneuous_gain() const noexcept {
  return VALowpass1P<SAMPLE_RATE>::get_instantaneuous_gain() /
         VALowpass1P<SAMPLE_RATE>::g;
}

_VA_HIGHPASS_1P_TEMPLATE
real_type _VA_HIGHPASS_1P_TYPE::get_instantaneuous_bias() const noexcept {
  return -VALowpass1P<SAMPLE_RATE>::get_instantaneuous_bias();
}

// VA Resonant Series =========================================================

_VA_RESONANT_SERIES_TEMPLATE
_VA_RESONANT_SERIES_TYPE::VAResonantSeries(real_type cutoff_freq,
                                           real_type resonance,
                                           real_type max_resonance) {
  this->max_resonance = max_resonance;
  set_resonance(resonance);
  set_freq(cutoff_freq);
}

_VA_RESONANT_SERIES_TEMPLATE
void _VA_RESONANT_SERIES_TYPE::reset() noexcept {
  for (uint32_t i = 0; i < N_FILTERS; i++) {
    one_pole_filters[i].reset();
  }
}

_VA_RESONANT_SERIES_TEMPLATE
void _VA_RESONANT_SERIES_TYPE::set_freq(real_type cutoff_freq) noexcept {
  for (uint32_t i = 0; i < N_FILTERS; i++) {
    one_pole_filters[i].set_freq(cutoff_freq);
  }

  // Computing G G^2 G^3 G^4 ...
  gain_powers[0] = one_pole_filters[0].get_instantaneuous_gain();
  for (uint32_t i = 1; i < N_FILTERS; i++) {
    gain_powers[i] = gain_powers[i - 1] * gain_powers[0];
  }
}

_VA_RESONANT_SERIES_TEMPLATE
void _VA_RESONANT_SERIES_TYPE::set_resonance(real_type resonance) noexcept {
  this->resonance = clip(resonance, 0.0f, 1.0f) * max_resonance;
}

_VA_RESONANT_SERIES_TEMPLATE
real_type _VA_RESONANT_SERIES_TYPE::process_sample(real_type input) noexcept {
  real_type u = compute_series_input(input);
  for (uint32_t i = 0; i < N_FILTERS; i++) {
    u = one_pole_filters[i].process_sample(u);
  }
  return u;
}

// VA Ladder 2P =======================================================

_VA_LADDER_2P_TEMPLATE
_VA_LADDER_2P_TYPE::VALadder2P(real_type cutoff_freq, real_type resonance,
                               real_type max_resonance)
    : VAResonantSeries<SAMPLE_RATE, 2, FILTER>(cutoff_freq, resonance,
                                               max_resonance) {}

_VA_LADDER_2P_TEMPLATE
real_type _VA_LADDER_2P_TYPE::compute_series_input(real_type input) noexcept {
  using Parent = VAResonantSeries<SAMPLE_RATE, 2, FILTER>;
  real_type g1 = Parent::gain_powers[0];
  real_type g2 = Parent::gain_powers[1];
  real_type s1 = Parent::one_pole_filters[0].get_instantaneuous_bias();
  real_type s2 = Parent::one_pole_filters[1].get_instantaneuous_bias();
  real_type k = Parent::resonance;

  return (input - k * (g1 * s1 + s2)) / (1 + g2 * k);
}

// VA Ladder 4P =======================================================

_VA_LADDER_4P_TEMPLATE
_VA_LADDER_4P_TYPE::VALadder4P(real_type cutoff_freq, real_type resonance)
    : VAResonantSeries<SAMPLE_RATE, 4, FILTER>(cutoff_freq, resonance, 3.8f) {
}  // Unstable for resonance >= 4

_VA_LADDER_4P_TEMPLATE
real_type _VA_LADDER_4P_TYPE::compute_series_input(real_type input) noexcept {
  using Parent = VAResonantSeries<SAMPLE_RATE, 4, FILTER>;
  real_type g1 = Parent::gain_powers[0];
  real_type g2 = Parent::gain_powers[1];
  real_type g3 = Parent::gain_powers[2];
  real_type g4 = Parent::gain_powers[3];
  real_type s1 = Parent::one_pole_filters[0].get_instantaneuous_bias();
  real_type s2 = Parent::one_pole_filters[1].get_instantaneuous_bias();
  real_type s3 = Parent::one_pole_filters[2].get_instantaneuous_bias();
  real_type s4 = Parent::one_pole_filters[3].get_instantaneuous_bias();
  real_type k = Parent::resonance;

  return (input - k * (g3 * s1 + g2 * s2 + g1 * s3 + s4)) / (1 + g4 * k);
}

// ========================================================================

}  // namespace audio_toolbox
