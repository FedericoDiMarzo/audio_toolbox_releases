#include "audio_toolbox/processors/distortions/bitcrush.hpp"

using namespace audio_toolbox;

BitCrush::BitCrush(uint8_t bit_depth, uint32_t downsample_factor, real_type clip_threshold)
{
    set_bit_depth(bit_depth);
    set_downsample_factor(downsample_factor);
    set_clip_threshold(clip_threshold);
    reset();
}

void BitCrush::reset() noexcept
{
    downsample_count = 0;
    last_sample = 0;
    next_sample = 0;
}

void BitCrush::set_bit_depth(uint8_t bit_depth) noexcept
{
    this->bit_depth = bit_depth > 23 ? 23 : bit_depth;
}

void BitCrush::set_downsample_factor(uint32_t downsample_factor) noexcept
{
    this->downsample_factor = downsample_factor;
}

void BitCrush::set_clip_threshold(real_type clip_threshold) noexcept
{
    this->clip_threshold = clip_threshold;
}

void BitCrush::process_sample(const real_type* input, real_type* output) noexcept
{
    /*
     * A union is a special data type available in C that allows to store
     * different types of data in the same memory location. You can define a
     * union with many members, but only one member can contain a value at any
     * given time.
     *
     * In the context of audio processing, a union is particularly useful for
     * manipulating the binary representation of a float. This is because a
     * union allows us to treat the same bits as both a float and an integer.
     *
     * For example, in the `process_sample` function, we use a union to apply
     * the bitcrush effect. We first treat the input sample as a float, then we
     * use the union to access the same bits as an integer. This allows us to
     * manipulate the bits directly, which is how the bitcrush effect is
     * achieved.
     *
     * After we're done manipulating the bits, we can use the union to treat the
     * bits as a float again, which is the format we need for the audio output.
     */
    union
    {
        float f;
        uint32_t i;
    } u;

    // Clipping
    u.f = std::max(-clip_threshold, std::min(clip_threshold, input[0]));

    // Downsampling
    downsample_count++;
    if (downsample_count >= downsample_factor)
    {
        downsample_count = 0;
        last_sample = next_sample;
        next_sample = u.f;
    }

    // Linear interpolation
    u.f = last_sample + (next_sample - last_sample) * ((float)downsample_count / downsample_factor);

    // Bitcrushing
    uint32_t mantissa = (u.i & 0x007FFFFF);
    mantissa = (mantissa >> bit_depth) << bit_depth;
    u.i = (u.i & 0xFF800000) | mantissa;

    output[0] = u.f;
}
