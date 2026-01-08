

namespace audio_toolbox {

_BASE_OSCILLATOR_TEMPLATE
_BASE_OSCILLATOR_TYPE::BaseOscillator(real_type frequency,
                                      real_type normalized_phase)
    : sample_rate(static_cast<real_type>(SAMPLE_RATE)) {
  set_frequency(frequency);
  set_phase(normalized_phase);
  reset();
}

_BASE_OSCILLATOR_TEMPLATE
void _BASE_OSCILLATOR_TYPE::set_frequency(real_type frequency) noexcept {
  this->frequency = clip(frequency, 0.0f, sample_rate / 2.0f);
}

_BASE_OSCILLATOR_TEMPLATE
real_type _BASE_OSCILLATOR_TYPE::get_frequency() const noexcept {
  return frequency;
}

_BASE_OSCILLATOR_TEMPLATE
void _BASE_OSCILLATOR_TYPE::set_phase(real_type phase) noexcept {
  this->phase = clip(phase, 0.0f, 1.0f);
}

_LOOKUP_OSCILLATOR_TEMPLATE
_LOOKUP_OSCILLATOR_TYPE::LookupOscillator(std::function<float(float)> function,
                                          real_type arg_min, real_type arg_max,
                                          real_type frequency,
                                          real_type normalized_phase)
    : _BASE_OSCILLATOR_TYPE(frequency, normalized_phase),
      lut(function, arg_min, arg_max) {
  set_frequency(frequency);
  reset();
}

_LOOKUP_OSCILLATOR_TEMPLATE
void _LOOKUP_OSCILLATOR_TYPE::reset() noexcept {
  table_idx =
      lut.get_arg_min() + lut.get_arg_range() * _BASE_OSCILLATOR_TYPE::phase;
}

_LOOKUP_OSCILLATOR_TEMPLATE
void _LOOKUP_OSCILLATOR_TYPE::set_frequency(real_type frequency) noexcept {
  _BASE_OSCILLATOR_TYPE::set_frequency(frequency);
  table_idx_increment =
      frequency / _BASE_OSCILLATOR_TYPE::sample_rate * lut.get_arg_range();
}

_LOOKUP_OSCILLATOR_TEMPLATE
real_type _LOOKUP_OSCILLATOR_TYPE::process_sample() noexcept {
  real_type y = lut(table_idx);
  table_idx += table_idx_increment;
  if (table_idx >= lut.get_arg_max()) {
    table_idx -= lut.get_arg_max();
  }
  return y;
}

}  // namespace audio_toolbox