#if AT_FFT_IMPLEMENTATION == 1


    #include "audio_toolbox/processors/spectral/fft.hpp"

using namespace audio_toolbox;

FFT::FFT(uint32_t fft_size)
    : fft_size(fft_size)
{
    if (!is_power_of_two(fft_size))
    {
        AT_THROW_M(std::invalid_argument, "FFT size must be a power of two.");
    }

    arm_rfft_fast_init_f32(&arm_rfft_instance, fft_size);
}

FFT::~FFT() {}

void FFT::process(const real_type* input, real_type* output) noexcept
{
    // Copying on process buffer to avoid changing the input buffer
    std::copy_n(input, fft_size, process_buffer.get_write_ptr());

    // Computing the FFT
    arm_rfft_fast_f32(&arm_rfft_instance, process_buffer.get_write_ptr(), output, 0);
}

void FFT::process_inverse(const real_type* input, real_type* output) noexcept
{
    // Copying on process buffer to avoid changing the input buffer
    std::copy_n(input, 2 * get_n_freqs(), process_buffer.get_write_ptr());

    // Computing the IFFT
    arm_rfft_fast_f32(&arm_rfft_instance, process_buffer.get_write_ptr(), output, 1);
}

#endif // AT_FFT_IMPLEMENTATION == 1