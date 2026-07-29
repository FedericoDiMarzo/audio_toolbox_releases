#include "audio_toolbox/math/random.hpp"

#include "audio_toolbox/core/types.hpp"

#if AT_ENABLE_EFFICIENT_RAND == 0

void audio_toolbox::set_random_seed(uint64_t seed)
{
    get_random_generator().seed(seed);
}

std::default_random_engine& audio_toolbox::get_random_generator()
{
    static std::default_random_engine rand_gen;
    return rand_gen;
}

real_type audio_toolbox::get_random_real(real_type min, real_type max)
{
    std::uniform_real_distribution<real_type> distribution(min, max);
    return distribution(get_random_generator());
}

int32_t audio_toolbox::get_random_int(int32_t min, int32_t max)
{
    std::uniform_int_distribution<int32_t> distribution(min, max);
    return distribution(get_random_generator());
}

std::vector<uint32_t> audio_toolbox::compute_random_permutation(uint32_t n)
{
    std::vector<uint32_t> seq = create_int_range(0u, n);
    auto& gen = get_random_generator();
    std::shuffle(seq.begin(), seq.end(), gen);
    return seq;
}

void audio_toolbox::WhiteNoiseGen::process(real_type* output, uint32_t len, real_type std) noexcept
{
    auto& gen = get_random_generator();
    for (uint32_t i = 0; i < len; i++)
    {
        output[i] = distribution(gen) * std;
    }
}


#else

int g_x1 = 0x67452301;
int g_x2 = 0xefcdab89;

real_type sample_uniform_distribution() noexcept
{
    float g_fScale = 1.0f / 0xffffffff;
    g_x1 ^= g_x2;
    g_x2 += g_x1;
    return g_x2 * g_fScale + 1.0f;
}

void audio_toolbox::set_random_seed(uint64_t seed)
{
    static const int g_x1_start = 0x67452301;
    static const int g_x2_start = 0xefcdab89;
    g_x1 = g_x1_start;
    g_x2 = g_x2_start;
    for (uint32_t i = 0; i < seed; i++)
    {
        sample_uniform_distribution();
    }
}

real_type audio_toolbox::get_random_real(real_type min, real_type max)
{
    real_type x = sample_uniform_distribution();
    // Map to [min, max]
    return x * (max - min) + min;
}

int32_t audio_toolbox::get_random_int(int32_t min, int32_t max)
{
    return std::floor(get_random_real(static_cast<real_type>(min), static_cast<real_type>(max) + 1.0f));
}

std::vector<uint32_t> audio_toolbox::compute_random_permutation(uint32_t n)
{
    auto seq = create_int_range<uint32_t>(0, n);
    for (uint32_t i = 0; i < n; ++i)
    {
        uint32_t j = get_random_int(0, n - 1);
        std::swap(seq[i], seq[j]);
    }
    return seq;
}

void audio_toolbox::WhiteNoiseGen::process(real_type* output, uint32_t len, real_type std) noexcept
{
    for (uint32_t i = 0; i < len; i++)
    {
        output[i] = get_random_real(-1.0f, 1.0f) * std;
    }
}

#endif

void audio_toolbox::randomize_phase(real_type* input, uint32_t n_freqs, real_type amt) noexcept
{
    if (amt <= 0.0)
        return;

    for (uint32_t i = 0; i < n_freqs; i++)
    {
        real_type re = input[2 * i];
        real_type im = input[2 * i + 1];

        // draw a random phase
        real_type phase_rand = get_random_real(0.0f, 2.0f * M_PI * amt);

        real_type c = compute_cos(phase_rand);
        real_type s = compute_sin(phase_rand);

        // We multiply by exp(j * phase_rand)
        input[2 * i] = re * c - im * s;
        input[2 * i + 1] = re * s + im * c;
    }
}
