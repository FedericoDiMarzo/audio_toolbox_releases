
namespace audio_toolbox {

_AUDIO_BUFFER_TEMPLATE
_AUDIO_BUFFER_TYPE::AudioBuffer() {
  buffer = (T*)pffft_aligned_malloc(LEN * CHANNELS * sizeof(T));
  clear();
}

_AUDIO_BUFFER_TEMPLATE
_AUDIO_BUFFER_TYPE::~AudioBuffer() { pffft_aligned_free(buffer); }

_AUDIO_BUFFER_TEMPLATE
T& _AUDIO_BUFFER_TYPE::operator[](uint32_t index) noexcept {
  return buffer[index];
}

_AUDIO_BUFFER_TEMPLATE
T* _AUDIO_BUFFER_TYPE::begin() noexcept { return buffer; }

_AUDIO_BUFFER_TEMPLATE
const T* _AUDIO_BUFFER_TYPE::begin() const noexcept { return buffer; }

_AUDIO_BUFFER_TEMPLATE
T* _AUDIO_BUFFER_TYPE::end() noexcept { return buffer + LEN * CHANNELS; }

_AUDIO_BUFFER_TEMPLATE
const T* _AUDIO_BUFFER_TYPE::end() const noexcept {
  return buffer + LEN * CHANNELS;
}

_AUDIO_BUFFER_TEMPLATE
const T* _AUDIO_BUFFER_TYPE::get_read_ptr() const noexcept { return buffer; }

_AUDIO_BUFFER_TEMPLATE
T* _AUDIO_BUFFER_TYPE::get_write_ptr() const noexcept { return buffer; }

_AUDIO_BUFFER_TEMPLATE
uint32_t _AUDIO_BUFFER_TYPE::get_len() const noexcept { return LEN; }

_AUDIO_BUFFER_TEMPLATE
uint32_t _AUDIO_BUFFER_TYPE::get_channels() const noexcept { return CHANNELS; }

_AUDIO_BUFFER_TEMPLATE
uint32_t _AUDIO_BUFFER_TYPE::size() const noexcept {
  return get_channels() * get_len();
}

_AUDIO_BUFFER_TEMPLATE
void _AUDIO_BUFFER_TYPE::copy_to(T* output) noexcept {
  std::copy(buffer, buffer + LEN * CHANNELS, output);
}

_AUDIO_BUFFER_TEMPLATE
void _AUDIO_BUFFER_TYPE::copy_from_channel(AudioBuffer<LEN, 1, T>& mono_buffer,
                                           uint32_t channel) noexcept {
  for (uint32_t i = 0; i < LEN; i++) {
    mono_buffer[i] = buffer[i * CHANNELS + channel];
  }
}

_AUDIO_BUFFER_TEMPLATE
void _AUDIO_BUFFER_TYPE::copy_to_channel(AudioBuffer<LEN, 1, T>& mono_buffer,
                                         uint32_t channel) noexcept {
  for (uint32_t i = 0; i < LEN; i++) {
    buffer[i * CHANNELS + channel] = mono_buffer[i];
  }
}

_AUDIO_BUFFER_TEMPLATE
template <typename... Buffers>
void _AUDIO_BUFFER_TYPE::interleave_from(Buffers&... buffers) noexcept {
  static_assert(sizeof...(Buffers) == CHANNELS,
                "Number of buffers must match number of channels");
  std::array<AudioBuffer<LEN, 1, T>*, CHANNELS> buffer_array = {&buffers...};
  for (uint32_t i = 0; i < LEN; i++) {
    for (uint32_t j = 0; j < CHANNELS; j++) {
      buffer[i * CHANNELS + j] = (*buffer_array[j])[i];
    }
  }
}

_AUDIO_BUFFER_TEMPLATE
void _AUDIO_BUFFER_TYPE::clear() noexcept {
  std::fill(buffer, buffer + LEN * CHANNELS, T(0));
}

_AUDIO_BUFFER_TEMPLATE
void _AUDIO_BUFFER_TYPE::scale(T gain) noexcept {
  apply_gain(buffer, LEN * CHANNELS, gain);
}

_AUDIO_BUFFER_TEMPLATE
void _AUDIO_BUFFER_TYPE::scale_db(T gain_db) noexcept {
  apply_gain_db(buffer, LEN * CHANNELS, gain_db);
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
_SEEKABLE_AUDIO_BUFFER_TYPE::SeekableAudioBuffer() {
  seek_ptr = this->get_read_ptr();
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
const T* _SEEKABLE_AUDIO_BUFFER_TYPE::get_seek_ptr() const noexcept {
  return seek_ptr;
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
uint32_t _SEEKABLE_AUDIO_BUFFER_TYPE::get_block_len() const noexcept {
  return BLOCK_LEN;
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
uint32_t _SEEKABLE_AUDIO_BUFFER_TYPE::get_samples_left() const noexcept {
  return samples_left;
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_TYPE::set_loop_len(uint32_t loop_len) noexcept {
  this->loop_len = std::min(loop_len, this->get_len());
  reset();
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_TYPE::reset() {
  seek_ptr = this->get_read_ptr();
  samples_left = loop_len;
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_TYPE::update() {
  if (samples_left >= BLOCK_LEN) {
    seek_ptr += BLOCK_LEN * CHANNELS;
    samples_left -= BLOCK_LEN;
  } else {
    reset();
  }
}

_SEEKABLE_AUDIO_BUFFER_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_TYPE::copy_block_to(T* output) noexcept {
  std::copy(seek_ptr, seek_ptr + BLOCK_LEN * CHANNELS, output);
}

void audio_buffer_to_complex(const real_type* interleaved_buffer,
                             std::complex<real_type>* complex_buffer,
                             uint32_t len, uint32_t n_CHANNELS) noexcept {
  auto* buff_0 = interleaved_buffer;
  auto* buff_1 = complex_buffer;
  for (uint32_t i = 0; i < len; i++) {
    for (uint32_t j = 0; j < n_CHANNELS; j++) {
      auto real = buff_0[2 * i * n_CHANNELS + j];
      auto imag = buff_0[(2 * i + 1) * n_CHANNELS + j];
      buff_1[i * n_CHANNELS + j] = std::complex<real_type>{real, imag};
    }
  }
}

template <uint32_t CHANNELS, uint32_t LEN>
void audio_buffer_to_complex(
    AudioBuffer<2 * LEN, CHANNELS, real_type>& interleaved_buffer,
    AudioBuffer<LEN, CHANNELS, std::complex<real_type>>&
        complex_buffer) noexcept {
  audio_buffer_to_complex(interleaved_buffer.get_read_ptr(),
                          complex_buffer.get_write_ptr(),
                          complex_buffer.get_len(), CHANNELS);
}

void audio_buffer_from_complex(real_type* interleaved_buffer,
                               const std::complex<real_type>* complex_buffer,
                               uint32_t len, uint32_t n_CHANNELS) noexcept {
  auto* buff_0 = complex_buffer;
  auto* buff_1 = interleaved_buffer;
  for (uint32_t i = 0; i < len; i++) {
    for (uint32_t j = 0; j < n_CHANNELS; j++) {
      auto x = buff_0[i * n_CHANNELS + j];
      buff_1[(2 * i) * n_CHANNELS + j] = x.real();
      buff_1[(2 * i + 1) * n_CHANNELS + j] = x.imag();
    }
  }
}

template <uint32_t CHANNELS, uint32_t LEN>
void audio_buffer_from_complex(
    AudioBuffer<2 * LEN, CHANNELS, real_type>& interleaved_buffer,
    AudioBuffer<LEN, CHANNELS, std::complex<real_type>>&
        complex_buffer) noexcept {
  audio_buffer_from_complex(interleaved_buffer.get_write_ptr(),
                            complex_buffer.get_read_ptr(),
                            complex_buffer.get_len(), CHANNELS);
}

}  // namespace audio_toolbox
