
namespace audio_toolbox {

AudioParameter::AudioParameter(real_type value, uint32_t sample_rate,
                               uint32_t block_size, float transition_ms)
    : sample_rate(sample_rate),
      block_size(block_size),
      target_value(value),
      current_value(value) {
  if ((transition_ms / 1000.0f) < (static_cast<real_type>(block_size) /
                                   static_cast<real_type>(sample_rate))) {
    AT_THROW_M(std::invalid_argument, "Transition time is too short");
  }

  set_transition_ms(transition_ms);
}

void AudioParameter::update() noexcept {
  current_value = exponential_smoothing(target_value, current_value, alpha);
}

void AudioParameter::set(real_type value) noexcept { target_value = value; }

real_type AudioParameter::get() const noexcept { return current_value.load(); }

void AudioParameter::set_transition_ms(real_type transition_ms) {
  alpha = alpha_from_time_const(
      transition_ms / 1000.0f,
      static_cast<real_type>(block_size) / static_cast<real_type>(sample_rate));
}

}  // namespace audio_toolbox
