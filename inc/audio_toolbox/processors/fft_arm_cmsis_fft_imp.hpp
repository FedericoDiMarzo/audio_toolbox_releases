
namespace audio_toolbox {

_FFT_TEMPLATE
_FFT_TYPE::FFT() { arm_rfft_fast_init_f32(&arm_rfft_instance, FFT_SIZE); }

_FFT_TEMPLATE
_FFT_TYPE::~FFT() {}

_FFT_TEMPLATE
void _FFT_TYPE::compute_fft(const real_type* input,
                            real_type* output) noexcept {
  // Copying on process buffer to avoid changing the input buffer
  std::copy_n(input, FFT_SIZE, process_buffer.get_write_ptr());

  // Computing the FFT
  arm_rfft_fast_f32(&arm_rfft_instance, process_buffer.get_write_ptr(), output,
                    0);
}

_FFT_TEMPLATE
void _FFT_TYPE::compute_ifft(const real_type* input,
                             real_type* output) noexcept {
  // Copying on process buffer to avoid changing the input buffer
  std::copy_n(input, 2 * N_FREQS, process_buffer.get_write_ptr());

  // Computing the IFFT
  arm_rfft_fast_f32(&arm_rfft_instance, process_buffer.get_write_ptr(), output,
                    1);

  // Scaling the output
  // apply_gain(output, FFT_SIZE, 1.0f / static_cast<real_type>(FFT_SIZE));
}

}  // namespace audio_toolbox
