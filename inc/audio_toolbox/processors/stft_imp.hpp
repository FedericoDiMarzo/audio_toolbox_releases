

namespace audio_toolbox {

_STFT_TEMPLATE
_STFT_TYPE::STFT() {
  init_window();
  reset();
}

_STFT_TEMPLATE
_STFT_TYPE::~STFT() {}

_STFT_TEMPLATE
void _STFT_TYPE::reset() noexcept {
  process_buffer.clear();
  stft_buffer.clear();
  ola_buffer.clear();
}

_STFT_TEMPLATE
void _STFT_TYPE::init_window() {
  switch (WIN_TYPE) {
    case STFTWindowType::HanningSqrt:
      window = hann_window<WIN_SIZE>(true);
      break;
    default:
      AT_THROW_M(std::invalid_argument, "Invalid window type.");
  }
}

_STFT_TEMPLATE
const std::array<real_type, WIN_SIZE>& _STFT_TYPE::get_window() const noexcept {
  return window;
}

_STFT_TEMPLATE
const real_type* _STFT_TYPE::get_window_ptr() const noexcept {
  return window.data();
}

_STFT_TEMPLATE
void _STFT_TYPE::process(const real_type* input, real_type* output) noexcept {
  auto* stft_buffer_p = stft_buffer.get_write_ptr();
  auto* process_buffer_p = process_buffer.get_write_ptr();

  // Updating the STFT buffer
  memmove(stft_buffer_p, stft_buffer_p + HOPSIZE, HOPSIZE * sizeof(real_type));
  std::copy(input, input + HOPSIZE, stft_buffer_p + HOPSIZE);

  // Applying the window
  for (uint32_t i = 0; i < WIN_SIZE; i++) {
    process_buffer_p[i] = stft_buffer_p[i] * window[i];
  }

  // Applying the FFT
  fft.compute_fft(process_buffer_p, process_buffer_p);

  // Copying the output
  std::copy(process_buffer_p, process_buffer_p + (2 * N_FREQS), output);
}

_STFT_TEMPLATE
void _STFT_TYPE::process_inverse(const real_type* input,
                                 real_type* output) noexcept {
  auto* process_buffer_p = process_buffer.get_write_ptr();
  auto* ola_buffer_p = ola_buffer.get_write_ptr();

  // Applying the inverse FFT
  fft.compute_ifft(input, process_buffer_p);

  // Applying the window
  for (uint32_t i = 0; i < WIN_SIZE; i++) {
    process_buffer_p[i] *= window[i];
  }

  // Overlap and add
  for (uint32_t i = 0; i < HOPSIZE; i++) {
    output[i] = process_buffer_p[i] + ola_buffer_p[i];
  }

  // Saving the overlap buffer
  std::copy(process_buffer_p + HOPSIZE, process_buffer_p + WIN_SIZE,
            ola_buffer_p);
}

_STFT_TEMPLATE
void _STFT_TYPE::compute_fft(const real_type* input,
                             real_type* output) noexcept {
  fft.compute_fft(input, output);
}

_STFT_TEMPLATE
void _STFT_TYPE::compute_ifft(const real_type* input,
                              real_type* output) noexcept {
  fft.compute_ifft(input, output);
}

}  // namespace audio_toolbox
