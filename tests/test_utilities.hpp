#ifndef _AUDIO_TOOLBOX_TEST_UTILITIES_H_
#define _AUDIO_TOOLBOX_TEST_UTILITIES_H_

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <complex>
#include <cstdint>
#include <random>
#include <thread>
#include <vector>

#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"

namespace audio_toolbox
{

/**
 * @brief Check if two arrays are equal.
 *
 * @param x First array.
 * @param y Second array.
 * @param n Length of the arrays.
 * @param eps Absolute tolerance.
 */
void expect_all_near(const real_type* x, const real_type* y, unsigned int n, real_type eps = 1e-6)
{
    for (unsigned int i = 0; i < n; i++)
    {
        EXPECT_NEAR(x[i], y[i], eps) << "i = " << i;
    }
}

/**
 * @brief Check if two complex arrays are equal.
 *
 * @param x First array.
 * @param y Second array.
 * @param n Length of the arrays.
 * @param eps Absolute tolerance.
 */
void expect_all_near_cpx(const std::complex<real_type>* x,
                         const std::complex<real_type>* y,
                         unsigned int n,
                         real_type eps)
{
    for (unsigned int i = 0; i < n; i++)
    {
        auto e = std::abs(x[i] - y[i]);
        EXPECT_NEAR(e, 0, eps) << "i = " << i << " x[i] = " << x[i] << " y[i] = " << y[i];
    }
}

/**
 * @brief Print an array.
 *
 * @param x Array to print.
 * @param n Length of the array.
 */
void print_array(const real_type* x, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++)
    {
        std::cout << x[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief Get an array of zeroes.
 *
 * @tparam T Type of the array.
 * @tparam N Size of the array.
 * @return Array of zeroes.
 */
template <typename T, unsigned int N>
std::array<T, N> get_zeros()
{
    std::array<T, N> x;
    x.fill(T(0));
    return x;
}

/**
 * @brief Get an array of ones.
 *
 * @tparam T Type of the array.
 * @tparam N Size of the array.
 * @return Array of ones.
 */
template <typename T, unsigned int N>
std::array<T, N> get_ones()
{
    std::array<T, N> x;
    x.fill(T(1));
    return x;
}

} // namespace audio_toolbox

#endif // _AUDIO_TOOLBOX_TEST_UTILITIES_H_