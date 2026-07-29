
namespace audio_toolbox
{

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
_SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::SeekableAudioBufferStatic()
{
    seek_ptr = this->get_read_ptr();
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
const T* _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::get_seek_ptr() const noexcept
{
    return seek_ptr;
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
uint32_t _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::get_block_len() const noexcept
{
    return BLOCK_LEN;
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
uint32_t _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::get_samples_left() const noexcept
{
    return samples_left;
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::set_loop_len(uint32_t loop_len) noexcept
{
    this->loop_len = std::min(loop_len, this->get_len());
    reset();
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::reset()
{
    seek_ptr = this->get_read_ptr();
    samples_left = loop_len;
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::update()
{
    if (samples_left >= BLOCK_LEN)
    {
        seek_ptr += BLOCK_LEN * CHANNELS;
        samples_left -= BLOCK_LEN;
    }
    else
    {
        reset();
    }
}

_SEEKABLE_AUDIO_BUFFER_STATIC_TEMPLATE
void _SEEKABLE_AUDIO_BUFFER_STATIC_TYPE::copy_block_to(T* output) noexcept
{
    std::copy(seek_ptr, seek_ptr + BLOCK_LEN * CHANNELS, output);
}

} // namespace audio_toolbox
