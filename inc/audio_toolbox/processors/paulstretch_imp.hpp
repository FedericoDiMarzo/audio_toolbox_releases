

namespace audio_toolbox {

_PAUL_STRETCH_TEMPLATE
_PAUL_STRETCH_TYPE::PaulStretch() { reset(); }

_PAUL_STRETCH_TEMPLATE
void _PAUL_STRETCH_TYPE ::reset() {
  rec_buffer.clear();
  freq_buff.clear();
  fft_buff.clear();
  ola_count = 0;
  rec_count = 0;

  // Reset all the STFT instances
  for (auto& stft_var : stft_array) {
    std::visit([](auto&& stft) { stft.reset(); }, stft_var);
  }
}

_PAUL_STRETCH_TEMPLATE
real_type _PAUL_STRETCH_TYPE ::process_sample(real_type x) noexcept {
  if (is_recording_) {
    rec_buffer.push(x);
    rec_count++;
    rec_count = std::min(rec_count, rec_buffer.size());
    return x;
  } else {
    return process_step();
  }
}

_PAUL_STRETCH_TEMPLATE
void _PAUL_STRETCH_TYPE ::toggle_recording() noexcept {
  // We reset before switching to recording mode
  if (!is_recording_) reset();

  is_recording_ = !is_recording_;
}

_PAUL_STRETCH_TEMPLATE
void _PAUL_STRETCH_TYPE ::set_recording(bool state) noexcept {
  if (state != is_recording_) toggle_recording();
}

_PAUL_STRETCH_TEMPLATE
bool _PAUL_STRETCH_TYPE ::is_recording() const noexcept {
  return is_recording_;
}

_PAUL_STRETCH_TEMPLATE
void _PAUL_STRETCH_TYPE ::set_seek_position(real_type seek) noexcept {
  seek_pos = clip(seek, 0.0f, 1.0f);
}

_PAUL_STRETCH_TEMPLATE
real_type _PAUL_STRETCH_TYPE ::get_seek_position() const noexcept {
  return seek_pos;
}

_PAUL_STRETCH_TEMPLATE
void _PAUL_STRETCH_TYPE ::set_phase_randomization_amount(
    real_type amt) noexcept {
  phase_rand_amt = clip(amt, 0.0f, 1.0f);
}

_PAUL_STRETCH_TEMPLATE
real_type _PAUL_STRETCH_TYPE ::get_phase_randomization_amount() const noexcept {
  return phase_rand_amt;
}

_PAUL_STRETCH_TEMPLATE
uint32_t _PAUL_STRETCH_TYPE ::get_num_recorded_samples() const noexcept {
  return rec_count;
}

_PAUL_STRETCH_TEMPLATE
inline void _PAUL_STRETCH_TYPE ::set_fft_index(uint32_t index) noexcept {
  current_fft_index = std::min(index, N - 1);
}

_PAUL_STRETCH_TEMPLATE
inline uint32_t _PAUL_STRETCH_TYPE ::get_fft_index() const noexcept {
  return current_fft_index;
}

_PAUL_STRETCH_TEMPLATE
inline uint32_t _PAUL_STRETCH_TYPE ::get_fft_size() const noexcept {
  return FFT_LEN_ARRAY[current_fft_index];
}

_PAUL_STRETCH_TEMPLATE
uint32_t _PAUL_STRETCH_TYPE ::get_ola_len() const noexcept {
  // 50% overlap
  return get_fft_size() / 2;
}

_PAUL_STRETCH_TEMPLATE
inline uint32_t _PAUL_STRETCH_TYPE ::get_n_freqs() const noexcept {
  return get_fft_size() / 2 + 1;
}

_PAUL_STRETCH_TEMPLATE
void _PAUL_STRETCH_TYPE::generate_new_frame() noexcept {
  std::visit(
      [this](auto&& stft) {
        // Total number of valid samples in rec_buffer
        uint32_t total_samples = rec_count;

        // Ensure frame fits
        if (total_samples < get_ola_len()) {
          return;
        }

        // Map seek_pos [0,1] -> [oldest, newest]
        uint32_t max_seek = total_samples - get_ola_len() - 2;
        uint32_t seek = static_cast<uint32_t>(seek_pos * max_seek);

        // Fill fft_buff in chronological order (oldest → newest)
        for (uint32_t i = 0; i < get_ola_len(); ++i) {
          // rec_buffer[0] is newest, so subtract from total_samples-1
          uint32_t i_reversed = (total_samples - 1) - (seek + i);
          fft_buff[i] = rec_buffer[i_reversed];
        }

        // FFT -> randomize phase -> IFFT
        stft.process(fft_buff.get_read_ptr(), freq_buff.get_write_ptr());
        randomize_phase(freq_buff.get_write_ptr(), get_n_freqs(),
                        phase_rand_amt);
        stft.process_inverse(freq_buff.get_read_ptr(),
                             fft_buff.get_write_ptr());
      },
      stft_array[current_fft_index]);
}

_PAUL_STRETCH_TEMPLATE
real_type _PAUL_STRETCH_TYPE ::process_step() noexcept {
  // We generate a new frame every half FFT window
  if (ola_count >= get_ola_len()) {
    ola_count = 0;
    generate_new_frame();
  }

  real_type y = fft_buff[ola_count];
  ola_count++;
  return y;
}

}  // namespace audio_toolbox
