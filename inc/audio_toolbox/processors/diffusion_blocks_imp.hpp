

namespace audio_toolbox {

_CHANNEL_EXPANDER_TEMPLATE
void _CHANNEL_EXPANDER_TYPE::process_sample(real_type input,
                                            real_type* output) noexcept {
  std::fill_n(output, CHANNELS, input);
}

_CHANNEL_EXPANDER_TEMPLATE
void _CHANNEL_EXPANDER_TYPE::process(const real_type* input, real_type* output,
                                     uint32_t length) noexcept {
  for (uint32_t i = 0; i < length; i++) {
    process_sample(input[i], output + i * CHANNELS);
  }
}

_MULTI_CHANNEL_EXPANDER_TEMPLATE
void _MULTI_CHANNEL_EXPANDER_TYPE::process_sample(const real_type* input,
                                                  real_type* output) noexcept {
  auto* y = output;
  for (uint32_t i = 0; i < CH_IN; i++) {
    std::fill_n(y, GROUP_LEN, input[i]);
    y += GROUP_LEN;
  }
}

_CHANNEL_REDUCER_TEMPLATE
void _CHANNEL_REDUCER_TYPE::process_sample(const real_type* input,
                                           real_type* output) noexcept {
  const real_type* x = input;
  for (uint32_t i = 0; i < CH_OUT; i++) {
    output[i] = x[i * GROUP_LEN];
  }
}

_CHANNEL_REDUCER_TEMPLATE
void _CHANNEL_REDUCER_TYPE::process(const real_type* input, real_type* output,
                                    uint32_t length) noexcept {
  for (uint32_t i = 0; i < length; i++) {
    process_sample(input + i * CH_IN, output + i * CH_OUT);
  }
}

_CHANNEL_REDUCER_AVG_TEMPLATE
void _CHANNEL_REDUCER_AVG_TYPE::process_sample(const real_type* input,
                                               real_type* output) noexcept {
  const real_type* x = input;
  for (uint32_t i = 0; i < CH_OUT; i++) {
    output[i] = compute_avg(x, GROUP_LEN);
    x += GROUP_LEN;
  }
}

_CHANNEL_REDUCER_AVG_TEMPLATE
void _CHANNEL_REDUCER_AVG_TYPE::process(const real_type* input,
                                        real_type* output,
                                        uint32_t length) noexcept {
  for (uint32_t i = 0; i < length; i++) {
    process_sample(input + i * CH_IN, output + i * CH_OUT);
  }
}

_CHANNEL_BLENDER_TEMPLATE
void _CHANNEL_BLENDER_TYPE::process_sample(const real_type* input,
                                           real_type* output) noexcept {
  matrix.dot(input, output);
}

_DIFFUSION_STEP_TEMPLATE
void _DIFFUSION_STEP_TYPE::reset() noexcept {
  delay.reset();
  std::fill_n(buff.begin(), CHANNELS, 0.0f);
}

_DIFFUSION_STEP_TEMPLATE
void _DIFFUSION_STEP_TYPE::process_sample(const real_type* input,
                                          real_type* output) noexcept {
  auto* tmp = buff.data();
  delay.process_sample(input, tmp);
  mixer.process_sample(tmp, output);
}

_DIFFUSION_STEP_TEMPLATE
DLY& _DIFFUSION_STEP_TYPE::get_delay() noexcept { return delay; }

_DIFFUSION_STEP_TEMPLATE
MIX& _DIFFUSION_STEP_TYPE::get_mixer() noexcept { return mixer; }

}  // namespace audio_toolbox
