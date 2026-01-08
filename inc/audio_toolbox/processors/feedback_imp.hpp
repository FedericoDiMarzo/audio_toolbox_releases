
namespace audio_toolbox {

_FEEDBACK_TEMPLATE
void _FEEDBACK_TYPE::set_feedback_gain(real_type gain) noexcept {
  feedback_gain = std::clamp(gain, 0.0f, 0.99999f);
}

_FEEDBACK_TEMPLATE
void _FEEDBACK_TYPE::reset() noexcept {
  for (auto& d : delay) d.reset();
  std::fill_n(state.begin(), CHANNELS, 0.0f);
}

_FEEDBACK_TEMPLATE
void _FEEDBACK_TYPE::process_sample(const real_type* input,
                                    real_type* output) noexcept {
  for (uint32_t i = 0; i < CHANNELS; i++) {
    state[i] = (1 - feedback_gain) * input[i] + feedback_gain * state[i];
  }
  delay.process_sample(state.data(), output);
  HouseholderMatrix<CHANNELS>::dot(output, output);
}

}  // namespace audio_toolbox
