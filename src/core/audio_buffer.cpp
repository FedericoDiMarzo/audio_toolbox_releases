#include "audio_toolbox/core/audio_buffer.hpp"

using namespace audio_toolbox;

AudioBuffer::AudioBuffer(uint32_t num_samples, uint32_t num_channels)
    : num_samples(num_samples),
      num_channels(num_channels)
{
    buffer = (real_type*)pffft_aligned_malloc(num_samples * num_channels * sizeof(real_type));
    clear();
}

AudioBuffer::~AudioBuffer()
{
    pffft_aligned_free(buffer);
}

void AudioBuffer::move_operator_implementation(AudioBuffer& other, AudioBuffer& target)
{
    target.buffer = other.buffer;
    target.num_samples = other.num_samples;
    target.num_channels = other.num_channels;
    other.buffer = nullptr;
    other.num_samples = 0;
    other.num_channels = 0;
}

AudioBuffer::AudioBuffer(AudioBuffer&& other)
{
    move_operator_implementation(other, *this);
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept
{
    if (this != &other)
    {
        pffft_aligned_free(buffer);
        move_operator_implementation(other, *this);
    }
    return *this;
}

void AudioBuffer::copy_to(real_type* output) noexcept
{
    std::copy(buffer, buffer + num_samples * num_channels, output);
}

void AudioBuffer::copy_from(const real_type* input) noexcept
{
    std::copy(input, input + num_samples * num_channels, buffer);
}

void AudioBuffer::copy_from_channel(AudioBuffer& mono_buffer, uint32_t channel) noexcept
{
    for (uint32_t i = 0; i < num_samples; i++)
    {
        mono_buffer[i] = buffer[i * num_channels + channel];
    }
}

void AudioBuffer::copy_to_channel(AudioBuffer& mono_buffer, uint32_t channel) noexcept
{
    for (uint32_t i = 0; i < num_samples; i++)
    {
        buffer[i * num_channels + channel] = mono_buffer[i];
    }
}

void AudioBuffer::clear() noexcept
{
    std::fill_n(buffer, size(), real_type(0));
}

void AudioBuffer::scale(real_type gain) noexcept
{
    apply_gain(buffer, size(), gain);
}

void AudioBuffer::scale_db(real_type gain_db) noexcept
{
    apply_gain_db(buffer, size(), gain_db);
}

void* AudioBuffer::pffft_aligned_malloc(size_t nb_bytes)
{
    void *p, *p0 = malloc(nb_bytes + 64);
    if (!p0)
        return (void*)0;
    p = (void*)(((size_t)p0 + 64) & (~((size_t)(64 - 1))));
    *((void**)p - 1) = p0;
    return p;
}

void AudioBuffer::pffft_aligned_free(void* p)
{
    if (p)
        free(*((void**)p - 1));
}
