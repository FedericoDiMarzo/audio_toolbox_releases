/*=====================================================================*
 *                   Copyright (C) 2012 Paul Mineiro                   *
 * All rights reserved.                                                *
 *                                                                     *
 * Redistribution and use in source and binary forms, with             *
 * or without modification, are permitted provided that the            *
 * following conditions are met:                                       *
 *                                                                     *
 *     * Redistributions of source code must retain the                *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer.                                       *
 *                                                                     *
 *     * Redistributions in binary form must reproduce the             *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer in the documentation and/or            *
 *     other materials provided with the distribution.                 *
 *                                                                     *
 *     * Neither the name of Paul Mineiro nor the names                *
 *     of other contributors may be used to endorse or promote         *
 *     products derived from this software without specific            *
 *     prior written permission.                                       *
 *                                                                     *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND              *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,         *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES               *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE             *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER               *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,                 *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES            *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE           *
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR                *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF          *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY              *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             *
 * POSSIBILITY OF SUCH DAMAGE.                                         *
 *                                                                     *
 * Contact: Paul Mineiro <paul@mineiro.com>                            *
 *=====================================================================*/

#ifndef __FASTMATH_C_
#define __FASTMATH_C_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

#include <stdint.h>

#include "audio_toolbox/core/types.hpp"

typedef union {
  real_type f;
  uint32_t i;
} fasthelper;

#ifndef __CAST
#define cast_u32 (uint32_t)
#endif  // __CAST

inline real_type fast_min(real_type x, real_type y) { return (x < y) ? x : y; }

inline real_type fast_max(real_type x, real_type y) { return (x > y) ? x : y; }

inline real_type fast_abs(real_type x) {
  // copy and re-interpret as 32 bit integer
  int casted = *(int*)&x;
  // clear highest bit
  casted &= 0x7FFFFFFF;

  // re-interpret as real_type
  return *(real_type*)&casted;
}

// This algorithm is dependant on IEEE representation and only works for 32 bits
inline real_type fast_sqrt(real_type x) {
  unsigned int i = *(unsigned int*)&x;
  // adjust bias
  i += 127 << 23;
  // approximation of square root
  i >>= 1;
  return *(real_type*)&i;
}

// The following code is the fast inverse square root implementation from Quake
// III Arena
inline real_type fast_invsqrt(real_type number) {
  long i;
  real_type x2, y;
  const real_type threehalfs = 1.5F;

  x2 = number * 0.5F;
  y = number;
  i = *(long*)&y;             // evil real_typeing point bit level hacking
  i = 0x5f3759df - (i >> 1);  // what the fuck?
  y = *(real_type*)&i;
  y = y * (threehalfs - (x2 * y * y));  // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be
  // removed

  return y;
}

/**********************************************************************/
// #ifdef FASTEXP
/**********************************************************************/

// Underflow of exponential is common practice in numerical routines,
// so handle it here.

inline real_type fast_pow2(real_type p) {
  real_type offset = (p < 0) ? 1.0f : 0.0f;
  real_type clipp = (p < -126) ? -126.0f : p;
  int w = (int)clipp;
  real_type z = clipp - w + offset;
  fasthelper v;
  v.f =
      cast_u32((1 << 23) * (clipp + 121.2740575f +
                            27.7280233f / (4.84252568f - z) - 1.49012907f * z));
  return v.f;
}

inline real_type fast_exp(real_type p) { return fast_pow2(1.442695040f * p); }

inline real_type faster_pow2(real_type p) {
  real_type clipp = (p < -126) ? -126.0f : p;
  fasthelper v;
  v.f = cast_u32((1 << 23) * (clipp + 126.94269504f));
  return v.f;
}

inline real_type faster_exp(real_type p) {
  return faster_pow2(1.442695040f * p);
}

// #endif // FASTEXP

/**********************************************************************/
// #ifdef FASTLOG
/**********************************************************************/

inline real_type fast_log2(real_type x) {
  real_type y;
  fasthelper vx, mx;
  vx.f = x;
  mx.i = (vx.i & 0x007FFFFF) | 0x3f000000;
  y = (real_type)vx.i;
  y *= 1.1920928955078125e-7f;

  return y - 124.22551499f - 1.498030302f * mx.f -
         1.72587999f / (0.3520887068f + mx.f);
}

inline real_type fast_pow(real_type x, real_type p) {
  return fast_pow2(p * fast_log2(x));
}

inline real_type fast_ln(real_type x) { return 0.69314718f * fast_log2(x); }

inline real_type fast_log(real_type x) { return 0.30102999f * fast_log2(x); }

inline real_type faster_log2(real_type x) {
  real_type y;
  fasthelper vx;
  vx.f = x;
  y = vx.i;
  y *= 1.1920928955078125e-7f;
  return y - 126.94269504f;
}

inline real_type faster_pow(real_type x, real_type p) {
  return faster_pow2(p * faster_log2(x));
}

inline real_type faster_ln(real_type x) {
  //  return 0.69314718f * faster_log2(x);

  real_type y;
  fasthelper vx;
  vx.f = x;
  y = vx.i;
  y *= 8.2629582881927490e-8f;
  return y - 87.989971088f;
}

inline real_type faster_log(real_type x) {
  return 0.30102999f * faster_log2(x);
}

// #endif // FASTLOG

/**********************************************************************/
// #ifdef FASTERF
/**********************************************************************/

// #include <math.h>

inline real_type fast_inverseerf(real_type x) {
  static const real_type invk = 0.30004578719350504f;
  static const real_type a = 0.020287853348211326f;
  static const real_type b = 0.07236892874789555f;
  static const real_type c = 0.9913030456864257f;
  static const real_type d = 0.8059775923760193f;

  real_type xsq = x * x;

  return invk * fast_log2((1.0f + x) / (1.0f - x)) +
         x * (a - b * xsq) / (c - d * xsq);
}

inline real_type faster_inverseerf(real_type x) {
  static const real_type invk = 0.30004578719350504f;

  return invk * faster_log2((1.0f + x) / (1.0f - x));
}
// #endif // FASTERF

/**********************************************************************/
// #ifdef FASTGAMMA
/**********************************************************************/

/* gamma/digamma functions only work for positive inputs */

inline real_type fast_lgamma(real_type x) {
  real_type logterm = fast_log(x * (1.0f + x) * (2.0f + x));
  real_type xp3 = 3.0f + x;

  return -2.081061466f - x + 0.0833333f / xp3 - logterm +
         (2.5f + x) * fast_log(xp3);
}

inline real_type faster_lgamma(real_type x) {
  return -0.0810614667f - x - faster_log(x) + (0.5f + x) * faster_log(1.0f + x);
}

inline real_type fast_digamma(real_type x) {
  real_type twopx = 2.0f + x;
  real_type logterm = fast_log(twopx);

  return (-48.0f + x * (-157.0f + x * (-127.0f - 30.0f * x))) /
             (12.0f * x * (1.0f + x) * twopx * twopx) +
         logterm;
}

inline real_type faster_digamma(real_type x) {
  real_type onepx = 1.0f + x;

  return -1.0f / x - 1.0f / (2 * onepx) + faster_log(onepx);
}

// #endif // FASTGAMMA

/**********************************************************************/
// #ifdef FASTHYPERBOLIC
/**********************************************************************/

inline real_type fast_sinh(real_type p) {
  return 0.5f * (fast_exp(p) - fast_exp(-p));
}

inline real_type faster_sinh(real_type p) {
  return 0.5f * (faster_exp(p) - faster_exp(-p));
}

inline real_type fast_cosh(real_type p) {
  return 0.5f * (fast_exp(p) + fast_exp(-p));
}

inline real_type faster_cosh(real_type p) {
  return 0.5f * (faster_exp(p) + faster_exp(-p));
}

inline real_type fast_tanh(real_type p) {
  return -1.0f + 2.0f / (1.0f + fast_exp(-2.0f * p));
}

inline real_type faster_tanh(real_type p) {
  return -1.0f + 2.0f / (1.0f + faster_exp(-2.0f * p));
}

// #endif // FASTHYPERBOLIC

/**********************************************************************/
// #ifdef FASTARCHYPERBOLIC
/**********************************************************************/

inline real_type fast_asin(real_type x) {
  const real_type halfpi = 1.5707963267948966f;
  const real_type a0 = 1.5707288;
  const real_type a1 = -0.2121144;
  const real_type a2 = 0.0742610;
  const real_type a3 = -0.0187293;

  // real_type xx = abs(x);

  return (halfpi -
          fast_sqrt(1 - x) * (a0 + a1 * x + a2 * x * x + a3 * x * x * x));
}

inline real_type fast_acos(real_type x) {
  real_type negate = (real_type)(x < 0);
  real_type ret = -0.0187293;
  x = fast_abs(x);
  ret = ret * x;
  ret = ret + 0.0742610;
  ret = ret * x;
  ret = ret - 0.2121144;
  ret = ret * x;
  ret = ret + 1.5707288;
  ret = ret * fast_sqrt(1.0 - x);
  ret = ret - 2 * negate * ret;
  return negate * 3.14159265358979 + ret;
}

// https://www.dsprelated.com/showarticle/1052.php
// Polynomial approximating arctangenet on the range -1,1.
// Max error < 0.005 (or 0.29 degrees)
inline real_type fast_atan(real_type z) {
  const real_type n1 = 0.97239411f;
  const real_type n2 = -0.19194795f;
  return (n1 + n2 * z * z) * z;
}

// http://pubs.opengroup.org/onlinepubs/009695399/functions/atan2.html
inline real_type fast_atan2(real_type y, real_type x) {
  const real_type pi = 3.1415926535897932384626433832795;
  const real_type ONEQTR_PI = pi / 4.0;
  const real_type THRQTR_PI = 3.0 * pi / 4.0;
  real_type r, angle;
  real_type abs_y = fast_abs(y) + 1e-10f;  // kludge to prevent 0/0 condition
  if (x < 0.0f) {
    r = (x + abs_y) / (abs_y - x);
    angle = THRQTR_PI;
  } else {
    r = (x - abs_y) / (x + abs_y);
    angle = ONEQTR_PI;
  }
  angle += (0.1963f * r * r - 0.9817f) * r;
  if (y < 0.0f)
    return (-angle);  // negate if in quad III or IV
  else
    return (angle);
}

// #endif // FASTARCHYPERBOLIC

/**********************************************************************/
// #ifdef FASTLAMBERT_W
/**********************************************************************/

// these functions compute the upper branch aka W_0

inline real_type fast_lambertw(real_type x) {
  static const real_type threshold = 2.26445f;

  real_type c = (x < threshold) ? 1.546865557f : 1.0f;
  real_type d = (x < threshold) ? 2.250366841f : 0.0f;
  real_type a = (x < threshold) ? -0.737769969f : 0.0f;

  real_type logterm = fast_log(c * x + d);
  real_type loglogterm = fast_log(logterm);

  real_type minusw = -a - logterm + loglogterm - loglogterm / logterm;
  real_type expminusw = fast_exp(minusw);
  real_type xexpminusw = x * expminusw;
  real_type pexpminusw = xexpminusw - minusw;

  return (2.0f * xexpminusw -
          minusw * (4.0f * xexpminusw - minusw * pexpminusw)) /
         (2.0f + pexpminusw * (2.0f - minusw));
}

inline real_type faster_lambertw(real_type x) {
  static const real_type threshold = 2.26445f;

  real_type c = (x < threshold) ? 1.546865557f : 1.0f;
  real_type d = (x < threshold) ? 2.250366841f : 0.0f;
  real_type a = (x < threshold) ? -0.737769969f : 0.0f;

  real_type logterm = faster_log(c * x + d);
  real_type loglogterm = faster_log(logterm);

  real_type w = a + logterm - loglogterm + loglogterm / logterm;
  real_type expw = faster_exp(-w);

  return (w * w + expw * x) / (1.0f + w);
}

inline real_type fast_lambertwexpx(real_type x) {
  static const real_type k = 1.1765631309f;
  static const real_type a = 0.94537622168f;

  real_type logarg = fast_max(x, k);
  real_type powarg = (x < k) ? a * (x - k) : 0;

  real_type logterm = fast_log(logarg);
  real_type powterm = faster_pow2(powarg);  // don't need accuracy here

  real_type w = powterm * (logarg - logterm + logterm / logarg);
  real_type logw = fast_log(w);
  real_type p = x - logw;

  return w * (2.0f + p + w * (3.0f + 2.0f * p)) /
         (2.0f - p + w * (5.0f + 2.0f * w));
}

inline real_type faster_lambertwexpx(real_type x) {
  static const real_type k = 1.1765631309f;
  static const real_type a = 0.94537622168f;

  real_type logarg = fast_max(x, k);
  real_type powarg = (x < k) ? a * (x - k) : 0;

  real_type logterm = faster_log(logarg);
  real_type powterm = faster_pow2(powarg);

  real_type w = powterm * (logarg - logterm + logterm / logarg);
  real_type logw = faster_log(w);

  return w * (1.0f + x - logw) / (1.0f + w);
}
// #endif // FASTLAMBERT_W

/**********************************************************************/
// #ifdef FASTSIGMOID
/**********************************************************************/

inline real_type fast_sigmoid(real_type x) {
  return 1.0f / (1.0f + fast_exp(-x));
}

inline real_type faster_sigmoid(real_type x) {
  return 1.0f / (1.0f + faster_exp(-x));
}
// #endif // FASTSIGMOID

/**********************************************************************/
// #ifdef FASTTRIG
/**********************************************************************/

// http://www.devmaster.net/forums/showthread.php?t=5784
// fast sine variants are for x \in [ -\pi, pi ]
// fast cosine variants are for x \in [ -\pi, pi ]
// fast tangent variants are for x \in [ -\pi / 2, pi / 2 ]
// "full" versions of functions handle the entire range of inputs
// although the range reduction technique used here will be hopelessly
// inaccurate for |x| >> 1000
//
// WARNING: fast_sinfull, fast_cosfull, and fasttanfull can be slower than
// libc calls on older machines (!) and on newer machines are only
// slighly faster.  however:
//   * vectorized versions are competitive
//   * faster full versions are competitive

inline real_type fast_sin(real_type x) {
  static const real_type fouroverpi = 1.2732395447351627f;
  static const real_type fouroverpisq = 0.40528473456935109f;
  static const real_type q = 0.78444488374548933f;
  fasthelper p, r, s, vx;
  uint32_t sign;

  p.f = 0.20363937680730309f;
  r.f = 0.015124940802184233f;
  s.f = -0.0032225901625579573f;

  vx.f = x;
  sign = vx.i & 0x80000000;
  vx.i = vx.i & 0x7FFFFFFF;

  real_type qpprox = fouroverpi * x - fouroverpisq * x * vx.f;
  real_type qpproxsq = qpprox * qpprox;

  p.i |= sign;
  r.i |= sign;
  s.i ^= sign;

  return q * qpprox + qpproxsq * (p.f + qpproxsq * (r.f + qpproxsq * s.f));
}

inline real_type faster_sin(real_type x) {
  static const real_type fouroverpi = 1.2732395447351627f;
  static const real_type fouroverpisq = 0.40528473456935109f;
  static const real_type q = 0.77633023248007499f;
  fasthelper p, vx;
  uint32_t sign;

  p.f = 0.22308510060189463f;
  vx.f = x;
  sign = vx.i & 0x80000000;
  vx.i &= 0x7FFFFFFF;

  real_type qpprox = fouroverpi * x - fouroverpisq * x * vx.f;

  p.i |= sign;

  return qpprox * (q + p.f * qpprox);
}

inline real_type fast_sinfull(real_type x) {
  static const real_type twopi = 6.2831853071795865f;
  static const real_type invtwopi = 0.15915494309189534f;

  // int k = x * invtwopi;
  real_type k = x * invtwopi;
  real_type half = (x < 0) ? -0.5f : 0.5f;
  return fast_sin((half + k) * twopi - x);
}

inline real_type faster_sinfull(real_type x) {
  static const real_type twopi = 6.2831853071795865f;
  static const real_type invtwopi = 0.15915494309189534f;

  // int k = x * invtwopi;
  real_type k = x * invtwopi;
  real_type half = (x < 0) ? -0.5f : 0.5f;
  return faster_sin((half + k) * twopi - x);
}

inline real_type fast_cos(real_type x) {
  static const real_type halfpi = 1.5707963267948966f;
  static const real_type halfpiminustwopi = -4.7123889803846899f;
  real_type offset = (x > halfpi) ? halfpiminustwopi : halfpi;
  return fast_sin(x + offset);
}

inline real_type faster_cos(real_type x) {
  static const real_type twooverpi = 0.63661977236758134f;
  static const real_type p = 0.54641335845679634f;
  fasthelper vx;
  vx.f = x;
  vx.i &= 0x7FFFFFFF;

  real_type qpprox = 1.0f - twooverpi * vx.f;

  return qpprox + p * qpprox * (1.0f - qpprox * qpprox);
}

inline real_type fast_cosfull(real_type x) {
  static const real_type halfpi = 1.5707963267948966f;
  return fast_sinfull(x + halfpi);
}

inline real_type faster_cosfull(real_type x) {
  static const real_type halfpi = 1.5707963267948966f;
  return faster_sinfull(x + halfpi);
}

inline real_type fast_tan(real_type x) {
  static const real_type halfpi = 1.5707963267948966f;
  return fast_sin(x) / fast_sin(x + halfpi);
}

inline real_type faster_tan(real_type x) {
  return faster_sin(x) / faster_cos(x);
}

inline real_type fast_tanfull(real_type x) {
  static const real_type twopi = 6.2831853071795865f;
  static const real_type invtwopi = 0.15915494309189534f;

  // int k = x * invtwopi;
  real_type k = x * invtwopi;
  real_type half = (x < 0) ? -0.5f : 0.5f;
  real_type xnew = x - (half + k) * twopi;

  return fast_sin(xnew) / fast_cos(xnew);
}

inline real_type faster_tanfull(real_type x) {
  static const real_type twopi = 6.2831853071795865f;
  static const real_type invtwopi = 0.15915494309189534f;

  // int k = x * invtwopi;
  real_type k = x * invtwopi;
  real_type half = (x < 0) ? -0.5f : 0.5f;
  real_type xnew = x - (half + k) * twopi;

  return faster_sin(xnew) / faster_cos(xnew);
}

// #endif // FASTTRIG

#pragma GCC diagnostic pop  // -Wstrict-aliasing
#pragma GCC diagnostic pop  // -Wuninitialized

#endif  // __FASTMATH_C_