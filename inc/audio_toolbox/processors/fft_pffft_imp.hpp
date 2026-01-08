
namespace audio_toolbox {

_FFT_TEMPLATE
_FFT_TYPE::FFT() {
  static_assert(is_power_of_two(FFT_SIZE), "FFT_SIZE must be a power of two.");

  pffft_setup = pffft_new_setup(FFT_SIZE, PFFFT_REAL);
}

_FFT_TEMPLATE
_FFT_TYPE::~FFT() { pffft_destroy_setup(pffft_setup); }

_FFT_TEMPLATE
void _FFT_TYPE::compute_fft(const real_type* input,
                            real_type* output) noexcept {
  pffft_transform_ordered(pffft_setup, input, output,
                          work_buffer.get_write_ptr(), PFFFT_FORWARD);

  // We need to move the Nyquist frequency at the correct place
  output[FFT_SIZE] = output[1];  // nq.real
  output[FFT_SIZE + 1] = 0.0f;   // nq.imag
  output[1] = 0.0f;              // dc.imag
}

_FFT_TEMPLATE
void _FFT_TYPE::compute_ifft(const real_type* input,
                             real_type* output) noexcept {
  // Computing the IFFT
  // compute_ifft_unscaled(input, output);

  auto* process_buffer_p = process_buffer.get_write_ptr();
  auto* work_buffer_p = work_buffer.get_write_ptr();

  // Copying the input buffer and moving back the Nyquist frequency
  std::copy(input, input + (2 * N_FREQS - 1), process_buffer_p);
  process_buffer_p[1] = process_buffer_p[FFT_SIZE];  // nq.imag

  // Applying the inverse FFT
  pffft_transform_ordered(pffft_setup, process_buffer_p, output, work_buffer_p,
                          PFFFT_BACKWARD);

  // Scaling the output
  apply_gain(output, FFT_SIZE, 1.0f / static_cast<real_type>(FFT_SIZE));
}

}  // namespace audio_toolbox
