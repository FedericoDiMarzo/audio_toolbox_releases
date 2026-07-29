#include "audio_toolbox/math/math.hpp"

// Switch between std and fast_math implementations
#if AT_ENABLE_APPROX_MATH == 0
real_type audio_toolbox::compute_sin(real_type x) noexcept
{
    return std::sin(x);
}

real_type audio_toolbox::compute_cos(real_type x) noexcept
{
    return std::cos(x);
}

real_type audio_toolbox::compute_tan(real_type x) noexcept
{
    return std::tan(x);
}

real_type audio_toolbox::compute_tanh(real_type x) noexcept
{
    return std::tanh(x);
}

real_type audio_toolbox::compute_exp(real_type x) noexcept
{
    return std::exp(x);
}

real_type audio_toolbox::compute_sqrt(real_type x) noexcept
{
    return std::sqrt(x);
}

real_type audio_toolbox::compute_log(real_type x) noexcept
{
    return std::log(x);
}

//  real_type audio_toolbox::compute_log10(real_type x) noexcept { return
//  std::log10(x);
// }

real_type audio_toolbox::compute_log2(real_type x) noexcept
{
    return std::log2(x);
}

real_type audio_toolbox::compute_pow(real_type x, real_type y) noexcept
{
    return std::pow(x, y);
}
#else
real_type audio_toolbox::compute_sin(real_type x) noexcept
{
    return fast_sin(x);
}

real_type audio_toolbox::compute_cos(real_type x) noexcept
{
    return fast_cos(x);
}

real_type audio_toolbox::compute_tan(real_type x) noexcept
{
    return fast_tan(x);
}

real_type audio_toolbox::compute_tanh(real_type x) noexcept
{
    return fast_tanh(x);
}

real_type audio_toolbox::compute_exp(real_type x) noexcept
{
    return fast_exp(x);
}

real_type audio_toolbox::compute_sqrt(real_type x) noexcept
{
    return fast_sqrt(x);
}

real_type audio_toolbox::compute_log(real_type x) noexcept
{
    return fast_log(x);
}

// TODO
//  real_type audio_toolbox::compute_log10(real_type x) noexcept { return
//  fast_log10(x);
// }

real_type audio_toolbox::compute_log2(real_type x) noexcept
{
    return fast_log2(x);
}

real_type audio_toolbox::compute_pow(real_type x, real_type y) noexcept
{
    return fast_pow(x, y);
}
#endif

real_type audio_toolbox::compute_db(real_type x) noexcept
{
    return 20.0f * std::log10(x);
}

real_type audio_toolbox::compute_inverse_db(real_type x) noexcept
{
    // The approximation is not accurate enough in the general case
    return std::pow(10.0f, x / 20.0f);
}

real_type audio_toolbox::compute_energy(const real_type* x, uint32_t n) noexcept
{
    real_type energy = std::inner_product(x, x + n, x, 0.0f);
    return energy;
}

real_type audio_toolbox::compute_avg(const real_type* x, uint32_t n) noexcept
{
    real_type avg = std::reduce(x, x + n, 0.0f) / n;
    return avg;
}

real_type audio_toolbox::compute_std(const real_type* x, uint32_t n) noexcept
{
    real_type avg = compute_avg(x, n);
    real_type std =
        std::reduce(x, x + n, 0.0f, [avg](real_type acc, real_type x) { return acc + (x - avg) * (x - avg); });
    return compute_sqrt(std / n);
}

real_type audio_toolbox::compute_mse(const real_type* x, real_type* y, uint32_t n) noexcept
{
    real_type mse = std::inner_product(x,
                                       x + n,
                                       y,
                                       0.0f,
                                       std::plus<>(),
                                       [](real_type x, real_type y) { return (x - y) * (x - y); });
    return mse / n;
}

real_type audio_toolbox::compute_mae(const real_type* x, real_type* y, uint32_t n) noexcept
{
    real_type mae =
        std::inner_product(x, x + n, y, 0.0f, std::plus<>(), [](real_type x, real_type y) { return std::abs(x - y); });
    return mae / n;
}

std::vector<real_type> audio_toolbox::hann_window(uint32_t n, bool apply_sqrt) noexcept
{
    std::vector<real_type> win(n);
    for (uint32_t i = 0; i < n; i++)
    {
        win[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / n));
        if (apply_sqrt)
            win[i] = sqrt(win[i]);
    }
    return win;
}

real_type audio_toolbox::linear_interpolation(real_type x0, real_type x1, real_type t) noexcept
{
    return x0 + (x1 - x0) * t;
}

real_type audio_toolbox::clip_max(real_type x, real_type max) noexcept
{
    return std::min(x, max);
}

real_type audio_toolbox::clip_min(real_type x, real_type min) noexcept
{
    return std::max(x, min);
}

real_type audio_toolbox::clip(real_type x, real_type min, real_type max) noexcept
{
    return std::min(std::max(x, min), max);
}

uint32_t audio_toolbox::compute_bin_from_freq(real_type freq, uint32_t sample_rate, uint32_t fft_size) noexcept
{
    uint32_t n_freqs = fft_size / 2 + 1;
    return static_cast<uint32_t>(2 * freq * n_freqs / sample_rate);
}

real_type audio_toolbox::compute_gain_at_freq(const real_type* x,
                                              real_type freq,
                                              uint32_t sample_rate,
                                              uint32_t fft_size) noexcept
{
    auto bin = compute_bin_from_freq(freq, sample_rate, fft_size);
    return compute_sqrt(x[bin] * x[bin] + x[2 * bin] * x[2 * bin]);
}

real_type
audio_toolbox::linear_map(real_type x, real_type x_min, real_type x_max, real_type y_min, real_type y_max) noexcept
{
    return y_min + (x - x_min) * (y_max - y_min) / (x_max - x_min);
}

real_type audio_toolbox::alpha_from_time_const(real_type tao, real_type period) noexcept
{
    return clip(period / tao, 0, 1);
}

real_type audio_toolbox::exponential_smoothing(real_type x, real_type y_last, real_type alpha) noexcept
{
    return alpha * x + (1 - alpha) * y_last;
}

void audio_toolbox::compute_ema(const real_type* x, real_type* y, uint32_t n, real_type alpha) noexcept
{
    y[0] = 0.0f;
    for (uint32_t i = 1; i < n; i++)
    {
        y[i] = exponential_smoothing(x[i], alpha, y[i - 1]);
    }
}

real_type audio_toolbox::frequency_prewarping(real_type cutoff_freq, real_type sample_rate) noexcept
{
    return sample_rate / M_PI * compute_tan(M_PI * cutoff_freq / sample_rate);
}

real_type audio_toolbox::exponential_mapping(real_type x, real_type min, real_type max) noexcept
{
    return min * compute_pow(max / min, x);
}
