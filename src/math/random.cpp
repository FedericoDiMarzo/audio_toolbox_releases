#include "audio_toolbox/math/random.hpp"

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"

using namespace audio_toolbox;

#if AT_ENABLE_EFFICIENT_RAND == 0

void audio_toolbox::set_random_seed(uint64_t seed) {
  get_random_generator().seed(seed);
}

std::default_random_engine& audio_toolbox::get_random_generator() {
  static std::default_random_engine rand_gen;
  return rand_gen;
}

real_type audio_toolbox::get_random_real(real_type min, real_type max) {
  std::uniform_real_distribution<real_type> distribution(min, max);
  return distribution(get_random_generator());
}

int32_t audio_toolbox::get_random_int(int32_t min, int32_t max) {
  std::uniform_int_distribution<int32_t> distribution(min, max);
  return distribution(get_random_generator());
}

#else

int g_x1 = 0x67452301;
int g_x2 = 0xefcdab89;

real_type sample_uniform_distribution() noexcept {
  float g_fScale = 1.0f / 0xffffffff;
  g_x1 ^= g_x2;
  g_x2 += g_x1;
  return g_x2 * g_fScale + 1.0f;
}

void audio_toolbox::set_random_seed(uint64_t seed) {
  static const int g_x1_start = 0x67452301;
  static const int g_x2_start = 0xefcdab89;
  g_x1 = g_x1_start;
  g_x2 = g_x2_start;
  for (uint32_t i = 0; i < seed; i++) {
    sample_uniform_distribution();
  }
}

real_type audio_toolbox::get_random_real(real_type min, real_type max) {
  real_type x = sample_uniform_distribution();
  // Map to [min, max]
  return x * (max - min) + min;
}

int32_t audio_toolbox::get_random_int(int32_t min, int32_t max) {
  return std::floor(get_random_real(static_cast<real_type>(min),
                                    static_cast<real_type>(max) + 1.0f));
}

#endif

void audio_toolbox::randomize_phase(real_type* input, uint32_t n_freqs,
                                    real_type amt) noexcept {
  if (amt <= 0.0) return;

  for (uint32_t i = 0; i < n_freqs; i++) {
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