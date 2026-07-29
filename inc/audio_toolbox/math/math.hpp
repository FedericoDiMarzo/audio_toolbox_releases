#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <string>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/fast_math.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323846f
#endif
namespace audio_toolbox
{

// Switch between std and fast_math implementations
// by using the AT_ENABLE_APPROX_MATH flag
real_type compute_sin(real_type x) noexcept;

real_type compute_cos(real_type x) noexcept;

real_type compute_tan(real_type x) noexcept;

real_type compute_tanh(real_type x) noexcept;

real_type compute_exp(real_type x) noexcept;

real_type compute_sqrt(real_type x) noexcept;

real_type compute_log(real_type x) noexcept;

real_type compute_log2(real_type x) noexcept;

real_type compute_pow(real_type x, real_type y) noexcept;

/**
 * @brief Compute the decibel value of a number.
 *
 * @param x The input value.
 * @return The decibel value.
 */
real_type compute_db(real_type x) noexcept;

/**
 * @brief Compute the inverse decibel value of a number.
 *
 * @param x The input value.
 * @return The inverse decibel value.
 */
real_type compute_inverse_db(real_type x) noexcept;

/**
 * @brief Compute the energy of a signal in the time domain.
 *
 * @param x The input signal.
 * @param n Length of the signal.
 * @return Energy of the signal.
 */
real_type compute_energy(const real_type* x, uint32_t n) noexcept;

/**
 * @brief Compute the average of a signal.
 *
 * @param x The input signal.
 * @param n Length of the signal.
 * @return Mean of the signal.
 */
real_type compute_avg(const real_type* x, uint32_t n) noexcept;

/**
 * @brief Compute the standard deviation of a signal.
 *
 * @param x The input signal.
 * @param n Length of the signal.
 * @return Standard deviation of the signal.
 */
real_type compute_std(const real_type* x, uint32_t n) noexcept;

/**
 * @brief Compute the mean squared error between two signals.
 *
 * @param x First signal.
 * @param y Second signal.
 * @param n Length of the signals.
 * @return Mean squared error.
 */
real_type compute_mse(const real_type* x, real_type* y, uint32_t n) noexcept;

/**
 * @brief Compute the mean absolute error between two signals.
 *
 * @param x First signal.
 * @param y Second signal.
 * @param n Length of the signals.
 * @return Mean absolute error.
 */
real_type compute_mae(const real_type* x, real_type* y, uint32_t n) noexcept;

/**
 * @brief Apply a gain to a signal.
 *
 * @tparam T Type of the signal. Must be real_type or std::complex<real_type>.
 * @param x Input signal.
 * @param n Length of the signal.
 * @param gain Linear gain.
 */
template <typename T>
void apply_gain(T* x, uint32_t n, real_type gain) noexcept;

/**
 * @brief Apply a gain to a signal in decibels.
 *
 * @tparam T Type of the signal. Must be real_type or std::complex<real_type>.
 * @param x Input signal.
 * @param n Length of the signal.
 * @param gain_db Gain in decibels.
 */
template <typename T>
void apply_gain_db(T* x, uint32_t n, real_type gain_db) noexcept;

/**
 * @brief Linear interpolation between two values.
 *
 * @param x0 First value.
 * @param x1 Second value.
 * @param t Interpolation index (between 0 and 1).
 * @return Interpolated value.
 */
real_type linear_interpolation(real_type x0, real_type x1, real_type t) noexcept;
/**
 * @brief Clip a value to a maximum value.
 *
 * @param x Value to clip.
 * @param max Maximum value.
 * @return real_type
 */
real_type clip_max(real_type x, real_type max) noexcept;

/**
 * @brief Clip a value to a minimum value.
 *
 * @param x Value to clip.
 * @param min Minimum value.
 * @return real_type
 */
real_type clip_min(real_type x, real_type min) noexcept;

/**
 * @brief Clip a value to a range.
 *
 * @param x Value to clip.
 * @param min Minimum value.
 * @param max Maximum value.
 * @return real_type
 */
real_type clip(real_type x, real_type min, real_type max) noexcept;

/**
 * @brief Compute the bin index from a frequency.
 *
 * @param freq The frequency.
 * @param sample_rate Sample rate.
 * @param fft_size Length of the FFT.
 * @return Bin index.
 */
uint32_t compute_bin_from_freq(real_type freq, uint32_t sample_rate, uint32_t fft_size) noexcept;

/**
 * @brief Compute the gain at a given frequency.
 *
 * @param x Input signal (fft_size / 2 + 1, 2).
 * @param freq Frequency in Hz.
 * @param sample_rate Sample rate in Hz.
 * @param fft_size Size of the FFT.
 * @return Linear gain.
 */
real_type compute_gain_at_freq(const real_type* x, real_type freq, uint32_t sample_rate, uint32_t fft_size) noexcept;

/**
 * @brief Map a value from one range to another.
 *
 * @param x Input value.
 * @param x_min Lower bound of the input range.
 * @param x_max Higher bound of the input range.
 * @param y_min Lower bound of the output range.
 * @param y_max Higher bound of the output range.
 * @return Mapped value.
 */
real_type linear_map(real_type x, real_type x_min, real_type x_max, real_type y_min, real_type y_max) noexcept;

/**
 * @brief Compute the time constant of the exponential
 * smoothing.
 *
 * @param tao Time constant in seconds.
 * @param period Update period in seconds.
 * @return Alpha value.
 */
real_type alpha_from_time_const(real_type tao, real_type period) noexcept;

/**
 * @brief Exponential smoothing.
 *
 * @param x Input value.
 * @param y_last Last smoothed value.
 * @param alpha Smoothing factor.
 * @return Smoothed output.
 */
real_type exponential_smoothing(real_type x, real_type y_last, real_type alpha) noexcept;

/**
 * @brief Compute the exponential moving average of a signal.
 *
 * @param x Input signal (n).
 * @param y Output signal (n).
 * @param n Length of the signals.
 * @param alpha Smoothing factor.
 */
void compute_ema(const real_type* x, real_type* y, uint32_t n, real_type alpha) noexcept;

/**
 * @brief Compute the absolute value of a signal.
 *
 * @tparam T Type of the signal. Must be real_type or std::complex<real_type>.
 * @param x Input signal.
 * @param y Output signal.
 * @param n Length of the signal.
 */
template <typename T>
void compute_abs(const T* x, T* y, uint32_t n) noexcept;

/**
 * @brief Compute the maximum value of a signal.
 *
 * @note 0 is returned for an empty array.
 * @tparam T Type of the signal.
 * @param x Input signal.
 * @param n Length of the signal.
 * @return Maximum value.
 */
template <typename T>
real_type compute_max(T* x, uint32_t n) noexcept;

/**
 * @brief Compute the minimum value of a signal.
 *
 * @note 0 is returned for an empty array.
 * @tparam T Type of the signal.
 * @param x Input signal.
 * @param n Length of the signal.
 * @return Minimum value.
 */
template <typename T>
real_type compute_min(T* x, uint32_t n) noexcept;

/**
 * @brief Apply a peak normalization to a signal.
 *
 * @tparam T Type of the signal. Must be real_type or std::complex<real_type>.
 * @param x Input signal.
 * @param y Output signal.
 * @param n Length of the signals.
 * @param peak Peak value.
 */
template <typename T>
void apply_peak_normalization(const T* x, T* y, uint32_t n, real_type peak = 1.0f) noexcept;

/**
 * @brief Compute a range of integers.
 *
 * @tparam T Type of the integers.
 * @param start First integer (included).
 * @param end Last integer (excluded).
 * @return Sequence of integers.
 */
template <typename T>
std::vector<T> create_int_range(T start, T end);

/**
 * @brief Compute the Hann window.
 *
 * @param n Length of the window.
 * @param apply_sqrt Apply the square root to the window.
 * @return Hann window.
 */
std::vector<real_type> hann_window(uint32_t n, bool apply_sqrt = false) noexcept;

/**
 * @brief Implements the frequency prewarping to compensate the bilinear
 * transform.
 *
 * @param cutoff_freq Analog cutoff frequency.
 * @param sample_rate Sample rate of the filter in Hz.
 * @return Digital cutoff frequency compensated for the bilinear transform.
 */
real_type frequency_prewarping(real_type cutoff_freq, real_type sample_rate) noexcept;

/**
 * @brief Maps a value between 0 and 1 to an exponential curve.
 * This function can be used to implement a logarithmic potentiometer.
 *
 * @param x Value between 0 and 1.
 * @param min Minimum value of the potentiometer.
 * @param max Maximum value of the potentiometer.
 * @return Mapped value.
 */
real_type exponential_mapping(real_type x, real_type min, real_type max) noexcept;

} // namespace audio_toolbox

#include "audio_toolbox/math/math_imp.hpp"
