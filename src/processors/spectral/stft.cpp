#include "audio_toolbox/processors/spectral/stft.hpp"

using namespace audio_toolbox;

STFT::STFT(uint32_t win_size, uint32_t fft_size, WindowType window_type)
    : fft(fft_size),
      window(win_size),
      process_buffer(get_num_freqs() * 2),
      stft_buffer(win_size),
      ola_buffer(get_hopsize())
{
    if (win_size > fft_size)
    {
        AT_THROW_M(std::invalid_argument, "Window size must be less than or equal to FFT size.");
    }

    init_window(window_type);
    reset();
}

void STFT::init_window(WindowType window_type)
{
    switch (window_type)
    {
        case WindowType::HanningSqrt:
            window.copy_from(hann_window(window.get_num_samples(), true).data());
            break;
        default:
            AT_THROW_M(std::invalid_argument, "Invalid window type.");
    }
}

void STFT::reset() noexcept
{
    process_buffer.clear();
    stft_buffer.clear();
    ola_buffer.clear();
}

void STFT::process(const real_type* input, real_type* output) noexcept
{
    auto* stft_buffer_p = stft_buffer.get_write_ptr();
    auto* process_buffer_p = process_buffer.get_write_ptr();

    // Updating the STFT buffer
    memmove(stft_buffer_p, stft_buffer_p + get_hopsize(), get_hopsize() * sizeof(real_type));
    std::copy(input, input + get_hopsize(), stft_buffer_p + get_hopsize());

    // Applying the window
    for (uint32_t i = 0; i < window.get_num_samples(); i++)
    {
        process_buffer_p[i] = stft_buffer_p[i] * window[i];
    }

    // Applying the FFT
    fft.process(process_buffer_p, process_buffer_p);

    // Copying the output
    std::copy(process_buffer_p, process_buffer_p + (2 * get_num_freqs()), output);
}

void STFT::process_inverse(const real_type* input, real_type* output) noexcept
{
    auto* process_buffer_p = process_buffer.get_write_ptr();
    auto* ola_buffer_p = ola_buffer.get_write_ptr();

    // Applying the inverse FFT
    fft.process_inverse(input, process_buffer_p);

    // Applying the window
    for (uint32_t i = 0; i < window.get_num_samples(); i++)
    {
        process_buffer_p[i] *= window[i];
    }

    // Overlap and add
    for (uint32_t i = 0; i < get_hopsize(); i++)
    {
        output[i] = process_buffer_p[i] + ola_buffer_p[i];
    }

    // Saving the overlap buffer
    std::copy(process_buffer_p + get_hopsize(), process_buffer_p + window.get_num_samples(), ola_buffer_p);
}
