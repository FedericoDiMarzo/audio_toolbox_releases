

namespace audio_toolbox {

_REVERB_DIFFUSED_TEMPLATE
_REVERB_DIFFUSE_TYPE::ReverbDiffuse() {
  set_lfo_frequency(LFO_INIT_FREQ, LFO_INIT_FREQ_DELTA);
  set_decay(INITIAL_DECAY);
  reset();
};

_REVERB_DIFFUSED_TEMPLATE
inline void _REVERB_DIFFUSE_TYPE::reset() noexcept {
  for (auto& t : tmp) t.clear();
  for (auto& d : diff_blocks) d.reset();
  feedback.reset();
}

_REVERB_DIFFUSED_TEMPLATE
void _REVERB_DIFFUSE_TYPE::process_sample(const real_type* input,
                                          real_type* output) {
  auto* tmp0 = tmp[0].get_write_ptr();
  auto* tmp1 = tmp[1].get_write_ptr();
  auto* tmp2 = tmp[2].get_write_ptr();
  auto* tmp3 = tmp[3].get_write_ptr();
  expander.process_sample(input, tmp0);
  std::copy_n(tmp0, DIFF_CH, tmp3);
  for (uint32_t i = 0; i < num_steps; ++i) {
    diff_blocks[i].process_sample(tmp0, tmp1);
    std::copy_n(tmp1, DIFF_CH, tmp2);
    apply_gain(tmp2, DIFF_CH, alpha_residual);
    apply_gain(tmp3, DIFF_CH, 1.0f - alpha_residual);
    std::transform(tmp2, tmp2 + DIFF_CH, tmp3, tmp3, std::plus<real_type>());
    // Switching diffusion in/out buffers
    std::swap(tmp0, tmp1);
  }

  // tmp3 holds the output of the diffusion blocks
  feedback.process_sample(tmp3, tmp0);
  reducer.process_sample(tmp0, output);
}

_REVERB_DIFFUSED_TEMPLATE
inline void _REVERB_DIFFUSE_TYPE::set_feedback_gain(real_type gain) noexcept {
  feedback.set_feedback_gain(gain);
}

_REVERB_DIFFUSED_TEMPLATE
inline void _REVERB_DIFFUSE_TYPE::set_alpha_residual(real_type alpha) noexcept {
  alpha_residual = std::clamp(alpha, 0.0f, 1.0f);
}

_REVERB_DIFFUSED_TEMPLATE
inline void _REVERB_DIFFUSE_TYPE::set_diffusion_steps(uint32_t steps) noexcept {
  num_steps = std::clamp(steps, 1u, MAX_DIFF_STEPS);
}

_REVERB_DIFFUSED_TEMPLATE
inline void _REVERB_DIFFUSE_TYPE::set_lfo_to_diffusion(
    real_type lfo_to_diff) noexcept {
  lfo_to_diffusion = clip_min(lfo_to_diff, 0.0f);
  foreach_delay([&](auto& d) { d.set_depth(lfo_to_diffusion); });
}

_REVERB_DIFFUSED_TEMPLATE
void _REVERB_DIFFUSE_TYPE::set_decay(real_type decay) noexcept {
  decay = clip(decay, 0.0f, 1.0f);
  auto d = linear_map(decay, 0.0f, 1.0f, 0.0f, DECAY_MAX);
  foreach_delay([&](auto& dly) {
    dly.set_random_range(d, d + DLY_RAND_DELTA);
    dly.randomize();
  });
}

_REVERB_DIFFUSED_TEMPLATE
void _REVERB_DIFFUSE_TYPE::set_lfo_frequency(real_type freq,
                                             real_type delta) noexcept {
  foreach_delay([&](auto& d) {
    d.set_frequency(freq + get_random_real(-delta / 2, delta / 2));
  });
}

_REVERB_DIFFUSED_TEMPLATE
template <typename FN>
void _REVERB_DIFFUSE_TYPE::foreach_delay(FN fn) {
  for (uint32_t i = 0; i < num_steps; ++i) {
    auto& dly = diff_blocks[i].get_delay();
    for (auto& d : dly) fn(d);
  }
}

}  // namespace audio_toolbox
