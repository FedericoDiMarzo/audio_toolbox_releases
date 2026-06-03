#if AT_FFT_IMPLEMENTATION == 0

    #include "audio_toolbox/processors/spectral/fft.hpp"


using namespace audio_toolbox;

FFT::FFT(uint32_t fft_size)
    : fft_size(fft_size)
{
    if (!is_power_of_two(fft_size))
    {
        AT_THROW_M(std::invalid_argument, "FFT size must be a power of two.");
    }
    pffft_setup = pffft_new_setup(fft_size, PFFFT_REAL);
}

FFT::~FFT()
{
    pffft_destroy_setup(pffft_setup);
}

void FFT::process(const real_type* input, real_type* output) noexcept
{
    pffft_transform_ordered(pffft_setup, input, output, work_buffer.get_write_ptr(), PFFFT_FORWARD);
    // We need to move the Nyquist frequency at the correct place
    output[fft_size] = output[1]; // nq.real
    output[fft_size + 1] = 0.0f;  // nq.imag
    output[1] = 0.0f;             // dc.imag
}

void FFT::process_inverse(const real_type* input, real_type* output) noexcept
{
    auto* process_buffer_p = process_buffer.get_write_ptr();
    auto* work_buffer_p = work_buffer.get_write_ptr();

    // Copying the input buffer and moving
    // back the Nyquist frequency
    std::copy(input, input + (2 * get_n_freqs() - 1), process_buffer_p);
    process_buffer_p[1] = process_buffer_p[fft_size]; // nq.imag

    // Applying the inverse FFT
    pffft_transform_ordered(pffft_setup, process_buffer_p, output, work_buffer_p, PFFFT_BACKWARD);

    // Scaling the output
    apply_gain(output, fft_size, 1.0f / static_cast<real_type>(fft_size));
}
#endif // AT_FFT_IMPLEMENTATION == 0