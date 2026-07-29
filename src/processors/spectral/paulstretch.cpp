#include "audio_toolbox/processors/spectral/paulstretch.hpp"

using namespace audio_toolbox;

PaulStretch::PaulStretch(uint32_t sample_rate, std::initializer_list<uint32_t> fft_lengths, real_type buffer_len_s)
    : buffer_len(next_power_of_two(static_cast<uint32_t>(buffer_len_s * sample_rate))),
      rec_buffer(buffer_len),
      // We use the move operator to initialize these buffers
      fft_buff(1),
      freq_buff(1)
{
    if (fft_lengths.size() == 0)
    {
        AT_THROW_M(std::invalid_argument, "At least one FFT length must be provided.");
    }
    // Initializing all the STFTs instances
    stfts.reserve(fft_lengths.size());
    for (uint32_t fft_len : fft_lengths)
    {
        stfts.push_back(std::make_unique<STFT>(fft_len, fft_len));
    }

    // Reinitialize the buffers with the correct sizes
    uint32_t max_fft_size = get_max_fft_size();
    uint32_t max_ola_len = max_fft_size / 2;
    uint32_t max_n_freqs = max_fft_size / 2 + 1;
    fft_buff = AudioBuffer(max_ola_len);
    freq_buff = AudioBuffer(2 * max_n_freqs);

    reset();
}

void PaulStretch::reset()
{
    rec_buffer.clear();
    freq_buff.clear();
    fft_buff.clear();
    ola_count = 0;
    rec_count = 0;

    // Reset all the STFT instances
    for (auto& stft : stfts)
    {
        stft->reset();
    }
}

uint32_t PaulStretch::get_max_fft_size() const noexcept
{
    return std::max_element(stfts.begin(),
                            stfts.end(),
                            [](const auto& a, const auto& b) { return a->get_fft_size() < b->get_fft_size(); })
        ->get()
        ->get_fft_size();
}

void PaulStretch::process_sample(const real_type* input, real_type* output) noexcept
{
    real_type x = input[0];
    real_type y;
    if (is_recording_)
    {
        rec_buffer.push(x);
        rec_count++;
        rec_count = std::min(rec_count, rec_buffer.size());
        y = x;
    }
    else
    {
        y = process_step();
    }
    output[0] = y;
}

void PaulStretch::generate_new_frame() noexcept
{
    STFT& stft = get_stft();

    // Total number of valid samples in rec_buffer
    uint32_t total_samples = rec_count;

    // Ensure frame fits
    if (total_samples < get_hopsize())
    {
        return;
    }

    // Map seek_pos [0,1] -> [oldest, newest]
    uint32_t max_seek = total_samples - get_hopsize() - 2;
    uint32_t seek = static_cast<uint32_t>(seek_pos * max_seek);

    // Fill fft_buff in chronological order (oldest → newest)
    for (uint32_t i = 0; i < get_hopsize(); ++i)
    {
        // rec_buffer[0] is newest, so subtract from total_samples-1
        uint32_t i_reversed = (total_samples - 1) - (seek + i);
        fft_buff[i] = rec_buffer[i_reversed];
    }

    // FFT -> randomize phase -> IFFT
    stft.process(fft_buff.get_read_ptr(), freq_buff.get_write_ptr());
    randomize_phase(freq_buff.get_write_ptr(), get_num_freqs(), phase_rand_amt);
    stft.process_inverse(freq_buff.get_read_ptr(), fft_buff.get_write_ptr());
}

real_type PaulStretch::process_step() noexcept
{
    // We generate a new frame every half FFT window
    if (ola_count >= get_hopsize())
    {
        ola_count = 0;
        generate_new_frame();
    }

    real_type y = fft_buff[ola_count];
    ola_count++;
    return y;
}
