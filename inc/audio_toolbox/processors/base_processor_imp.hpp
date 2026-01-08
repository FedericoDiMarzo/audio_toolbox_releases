
namespace audio_toolbox {

_BASE_PROCESSOR_TEMPLATE
void _BASE_PROCESSOR_TYPE::process(const T* input, T* output,
                                   uint32_t length) noexcept {
  for (uint32_t i = 0; i < length; i++) {
    static_cast<P*>(this)->process_sample(input + i * IN_CHANNELS,
                                          output + i * OUT_CHANNELS);
  }
}

_MONO_PROCESSOR_TEMPLATE
void _MONO_PROCESSOR_TYPE::process(const T* input, T* output,
                                   uint32_t length) noexcept {
  for (uint32_t i = 0; i < length; i++) {
    output[i] = static_cast<P*>(this)->process_sample(input[i]);
  }
}

_MONO_TO_MULTI_TEMPLATE
void _MONO_TO_MULTI_TYPE::reset() noexcept {
  for (auto& b : blocks) b.reset();
}

_MONO_TO_MULTI_TEMPLATE
P& _MONO_TO_MULTI_TYPE::operator[](uint32_t index) noexcept {
  return blocks[index];
}

_MONO_TO_MULTI_TEMPLATE
auto _MONO_TO_MULTI_TYPE::begin() noexcept { return blocks.begin(); }

_MONO_TO_MULTI_TEMPLATE
auto _MONO_TO_MULTI_TYPE::end() noexcept { return blocks.end(); }

_MONO_TO_MULTI_TEMPLATE
void _MONO_TO_MULTI_TYPE::process_sample(const T* input, T* output) noexcept {
  auto* x = input;
  auto* y = output;
  for (uint32_t i = 0; i < CHANNELS; i++) {
    *y = blocks[i].process_sample(*x);
    x++;
    y++;
  }
}

_MONO_TO_MULTI_TEMPLATE
void _MONO_TO_MULTI_TYPE::process(const T* input, T* output,
                                  uint32_t length) noexcept {
  for (uint32_t i = 0; i < length; i++) {
    process_sample(input + i * CHANNELS, output + i * CHANNELS);
  }
}

}  // namespace audio_toolbox