#include "audio_toolbox/core/seekable_audio_buffer.hpp"

using namespace audio_toolbox;

SeekableAudioBuffer::SeekableAudioBuffer(uint32_t num_samples, uint32_t num_channels, uint32_t update_block_len)
    : AudioBuffer(num_samples, num_channels),
      loop_len(num_samples),
      samples_left(num_samples),
      seek_ptr(get_read_ptr()),
      update_block_len(update_block_len)
{
}

void SeekableAudioBuffer::copy_block_to(real_type* output) noexcept
{
    std::copy(seek_ptr, seek_ptr + update_block_len * get_num_channels(), output);
}

void SeekableAudioBuffer::reset()
{
    seek_ptr = get_read_ptr();
    samples_left = loop_len;
}

void SeekableAudioBuffer::update()
{
    if (samples_left >= update_block_len)
    {
        seek_ptr += update_block_len * get_num_channels();
        samples_left -= update_block_len;
    }
    else
    {
        reset();
    }
}
