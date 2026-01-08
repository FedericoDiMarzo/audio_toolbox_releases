

namespace audio_toolbox {

_BASIC_CHORUS_TEMPLATE
_BASIC_CHORUS_TYPE::BasicChorus(real_type frequency, real_type depth,
                                real_type delay, real_type mix) {
  set_frequency(frequency);
  set_depth(depth);
  set_delay(delay);
  set_mix(mix);
  randomize_phases();
  reset();
};

_BASIC_CHORUS_TEMPLATE
void _BASIC_CHORUS_TYPE::reset() noexcept {
  for (auto& d : delays) d.reset();
}

_BASIC_CHORUS_TEMPLATE
real_type _BASIC_CHORUS_TYPE::process_sample(real_type input) noexcept {
  real_type output = 0.0;
  for (auto& d : delays) output += d.process_sample(input);
  return mix * (output / NUM_VOICES) + (1.0 - mix) * input;
}

_BASIC_CHORUS_TEMPLATE
void _BASIC_CHORUS_TYPE::set_delay(real_type delay) noexcept {
  for (auto& d : delays) d.set_delay(delay);
}

_BASIC_CHORUS_TEMPLATE
void _BASIC_CHORUS_TYPE::set_frequency(real_type frequency) noexcept {
  for (auto& d : delays) d.set_frequency(frequency);
}

_BASIC_CHORUS_TEMPLATE
void _BASIC_CHORUS_TYPE::set_depth(real_type depth) noexcept {
  for (auto& d : delays) d.set_depth(depth);
}

_BASIC_CHORUS_TEMPLATE
void _BASIC_CHORUS_TYPE::set_mix(real_type mix) noexcept { this->mix = mix; }

_BASIC_CHORUS_TEMPLATE
void _BASIC_CHORUS_TYPE::randomize_phases() noexcept {
  for (auto& d : delays) d.set_phase(get_random_real(0.0, M_PI_2));
}

}  // namespace audio_toolbox
