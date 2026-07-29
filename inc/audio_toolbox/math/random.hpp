#pragma once

#include <cstdint>
#include <random>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

#if AT_ENABLE_EFFICIENT_RAND == 0

/**
 * @brief Get the random generator object.
 *
 * @note The same static random generator is returned each time.
 * @return Random generator.
 */
std::default_random_engine& get_random_generator();

#endif

/**
 * @brief Set the random seed.
 *
 * @param seed New random seed.
 */
void set_random_seed(uint64_t seed);
/**
 * @brief Compute a random permutation of integers.
 *
 * @param n Number of integers to permute.
 * @return Random permutation.
 */
std::vector<uint32_t> compute_random_permutation(uint32_t n);

/**
 * @brief Get a random real number in the range [min, max).
 *
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Random real number.
 */
real_type get_random_real(real_type min, real_type max);

/**
 * @brief Get a random integer in the range [min, max].
 *
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Random integer.
 */
int32_t get_random_int(int32_t min, int32_t max);

/**
 * @brief Randomize the phase of the input buffer.
 *
 * @param input Input buffer (n_freqs, 2).
 * @param n_freqs Number of complex frequencies.
 * @param amt Amount of phase randomization in the range [0, 1].
 */
void randomize_phase(real_type* input, uint32_t n_freqs, real_type amt = 1.0f) noexcept;

/**
 * @brief White noise generator.
 */
class WhiteNoiseGen
{
  public:
    AT_DISABLE_COPY_AND_MOVE_M(WhiteNoiseGen);

    /**
     * @brief Construct a new White Noise Gen object.
     */
    WhiteNoiseGen()
        : distribution(0.0f, 1.0f){};

    /**
     * @brief Generate white noise in the buffer.
     *
     * @param output Buffer to fill with white noise.
     * @param len Length of the buffer.
     * @param std Standard deviation of the white noise.
     */
    void process(real_type* output, uint32_t len, real_type std = 1.0f) noexcept;

  private:
    /**
     * @brief Gaussian distribution used to generate white noise.
     */
    std::normal_distribution<real_type> distribution;
};

} // namespace audio_toolbox
