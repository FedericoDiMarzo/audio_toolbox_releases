
#include "audio_toolbox/processors/biquad.hpp"

#include <stdexcept>

#include "audio_toolbox/math/math.hpp"

using namespace audio_toolbox;

/*
  More infors about these formulas can be found at:
  https://www.w3.org/TR/audio-eq-cookbook
*/

real_type compute_A(real_type gain_db) { return std::pow(10, gain_db / 40); }

real_type compute_omega(real_type frequency, real_type sample_rate) {
  return 2 * M_PI * frequency / sample_rate;
}

real_type compute_alpha(real_type omega, real_type q) {
  return compute_sin(omega) / (2 * q);
}

audio_toolbox::Biquad::Biquad(real_type b0, real_type b1, real_type b2,
                              real_type a1, real_type a2) {
  set(b0, b1, b2, a1, a2);
}

audio_toolbox::Biquad::Biquad(real_type b0, real_type b1, real_type b2,
                              real_type a0, real_type a1, real_type a2) {
  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::set(real_type b0, real_type b1, real_type b2,
                                real_type a1, real_type a2) {
  B[0] = b0;
  B[1] = b1;
  B[2] = b2;
  A[0] = a1;
  A[1] = a2;
  wz[0] = 0;
  wz[1] = 0;
}

audio_toolbox::Biquad::Biquad(BiquadFilterType type, uint32_t sample_rate,
                              real_type frequency, real_type q,
                              real_type gain_db) {
  real_type A = compute_A(gain_db);
  real_type omega = compute_omega(frequency, sample_rate);
  real_type alpha = compute_alpha(omega, q);
  real_type cs = compute_cos(omega);

  switch (type) {
    case BiquadFilterType::Lowpass:
      design_lowpass(alpha, cs);
      break;

    case BiquadFilterType::Highpass:
      design_highpass(alpha, cs);
      break;

    case BiquadFilterType::Lowshelf:
      design_lowshelf(A, alpha, cs);
      break;

    case BiquadFilterType::Highshelf:
      design_highshelf(A, alpha, cs);
      break;

    case BiquadFilterType::Bandpass:
      design_bandpass(alpha, cs);
      break;

    case BiquadFilterType::Notch:
      design_notch(alpha, cs);
      break;

    case BiquadFilterType::Peak:
      design_peak(A, alpha, cs);
      break;

    case BiquadFilterType::Allpass:
      design_allpass(alpha, cs);
      break;

    default:
      AT_THROW_M(std::invalid_argument, "Unknown filter type.");
  }
}

real_type audio_toolbox::Biquad::process_sample(real_type x) noexcept {
  // Direct form II transposed
  real_type y;
  y = B[0] * x + wz[0];
  wz[0] = B[1] * x - A[0] * y + wz[1];
  wz[1] = B[2] * x - A[1] * y;

  return y;
}

std::vector<std::complex<real_type> > audio_toolbox::Biquad::poles() {
  std::vector<std::complex<real_type> > poles;

  std::complex<real_type> b2(A[0] * A[0], 0);
  std::complex<real_type> ds = std::sqrt(b2 - 4 * A[1]);

  poles.push_back(0.5f * (-A[0] + ds));
  poles.push_back(0.5f * (-A[0] - ds));

  return poles;
}

std::vector<std::complex<real_type> > audio_toolbox::Biquad::zeros() {
  std::vector<std::complex<real_type> > zeros;

  std::complex<real_type> b2(B[1] * B[1], 0);
  std::complex<real_type> ds = std::sqrt(b2 - 4 * B[0] * B[2]);

  zeros.push_back(0.5f * (-B[1] + ds) / B[0]);
  zeros.push_back(0.5f * (-B[1] - ds) / B[0]);

  return zeros;
}

bool audio_toolbox::Biquad::is_stable() {
  bool stable = true;
  std::vector<std::complex<real_type> > ps = poles();
  for (size_t i = 0; i < ps.size(); i++) {
    stable = stable & (std::abs(ps[i]) < 1);
  }
  return stable;
}

void audio_toolbox::Biquad::design_lowpass(real_type alpha, real_type cs) {
  real_type b0 = (1 - cs) / 2;
  real_type b1 = 1 - cs;
  real_type b2 = (1 - cs) / 2;
  real_type a0 = 1 + alpha;
  real_type a1 = -2 * cs;
  real_type a2 = 1 - alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_highpass(real_type alpha, real_type cs) {
  real_type b0 = (1 + cs) / 2;
  real_type b1 = -(1 + cs);
  real_type b2 = (1 + cs) / 2;
  real_type a0 = 1 + alpha;
  real_type a1 = -2 * cs;
  real_type a2 = 1 - alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_lowshelf(real_type A, real_type alpha,
                                            real_type cs) {
  real_type b0 = A * ((A + 1) - (A - 1) * cs + 2 * compute_sqrt(A) * alpha);
  real_type b1 = 2 * A * ((A - 1) - (A + 1) * cs);
  real_type b2 = A * ((A + 1) - (A - 1) * cs - 2 * compute_sqrt(A) * alpha);
  real_type a0 = (A + 1) + (A - 1) * cs + 2 * compute_sqrt(A) * alpha;
  real_type a1 = -2 * ((A - 1) + (A + 1) * cs);
  real_type a2 = (A + 1) + (A - 1) * cs - 2 * compute_sqrt(A) * alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_highshelf(real_type A, real_type alpha,
                                             real_type cs) {
  real_type b0 = A * ((A + 1) + (A - 1) * cs + 2 * compute_sqrt(A) * alpha);
  real_type b1 = -2 * A * ((A - 1) + (A + 1) * cs);
  real_type b2 = A * ((A + 1) + (A - 1) * cs - 2 * compute_sqrt(A) * alpha);
  real_type a0 = (A + 1) - (A - 1) * cs + 2 * compute_sqrt(A) * alpha;
  real_type a1 = 2 * ((A - 1) - (A + 1) * cs);
  real_type a2 = (A + 1) - (A - 1) * cs - 2 * compute_sqrt(A) * alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_bandpass(real_type alpha, real_type cs) {
  real_type b0 = alpha;
  real_type b1 = 0;
  real_type b2 = -alpha;
  real_type a0 = 1 + alpha;
  real_type a1 = -2 * cs;
  real_type a2 = 1 - alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_notch(real_type alpha, real_type cs) {
  real_type b0 = 1;
  real_type b1 = -2 * cs;
  real_type b2 = 1;
  real_type a0 = 1 + alpha;
  real_type a1 = -2 * cs;
  real_type a2 = 1 - alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_peak(real_type A, real_type alpha,
                                        real_type cs) {
  real_type b0 = 1 + alpha * A;
  real_type b1 = -2 * cs;
  real_type b2 = 1 - alpha * A;
  real_type a0 = 1 + alpha / A;
  real_type a1 = -2 * cs;
  real_type a2 = 1 - alpha / A;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

void audio_toolbox::Biquad::design_allpass(real_type alpha, real_type cs) {
  real_type b0 = 1 - alpha;
  real_type b1 = -2 * cs;
  real_type b2 = 1 + alpha;
  real_type a0 = 1 + alpha;
  real_type a1 = -2 * cs;
  real_type a2 = 1 - alpha;

  set(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}