#ifndef _AUDIO_TOOLBOX_VIRTUAL_ANALOG_FILTERS_1P_H_
#define _AUDIO_TOOLBOX_VIRTUAL_ANALOG_FILTERS_1P_H_

/*
  Implementation readapted from:
  https://github.com/FedericoDiMarzo/stm32-monosynth

  Credits:
  https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf
  https://www.kvraudio.com/forum/viewtopic.php?t=368466
*/

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/fast_math.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/processors/base_processor.hpp"

#define _VA_LOWPASS_1P_TYPE VALowpass1P<SAMPLE_RATE>
#define _VA_LOWPASS_1P_TEMPLATE template <uint32_t SAMPLE_RATE>

#define _VA_HIGHPASS_1P_TYPE VAHighpass1P<SAMPLE_RATE>
#define _VA_HIGHPASS_1P_TEMPLATE template <uint32_t SAMPLE_RATE>

#define _VA_RESONANT_SERIES_TYPE \
  VAResonantSeries<SAMPLE_RATE, N_FILTERS, ONE_POLE_FILTER>
#define _VA_RESONANT_SERIES_TEMPLATE \
  template <uint32_t SAMPLE_RATE, uint32_t N_FILTERS, typename ONE_POLE_FILTER>

#define _VA_LADDER_2P_TYPE VALadder2P<SAMPLE_RATE, FILTER>
#define _VA_LADDER_2P_TEMPLATE template <uint32_t SAMPLE_RATE, typename FILTER>

#define _VA_LADDER_4P_TYPE VALadder4P<SAMPLE_RATE, FILTER>
#define _VA_LADDER_4P_TEMPLATE template <uint32_t SAMPLE_RATE, typename FILTER>

namespace audio_toolbox {

// VA Base ==============================================================

/**
 * @brief Abstract base class for audio filters.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
class AudioFilterBase : public MonoProcessor<AudioFilterBase<SAMPLE_RATE>> {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(AudioFilterBase);

  AudioFilterBase() = default;

  virtual ~AudioFilterBase() = default;

  virtual void reset() noexcept = 0;

  virtual real_type process_sample(real_type input) noexcept = 0;

  /**
   * @brief Set the cutoff frequency of the filter.
   *
   * @param cutoff_freq The cutoff frequency in Hz.
   */
  virtual void set_freq(real_type cutoff_freq) noexcept = 0;

  /**
   * @brief Set the resonance of the filter.
   *
   * @param resonance The resonance value. Between 0 and 1.
   */
  virtual void set_resonance(real_type resonance) noexcept = 0;
};

/**
 * @brief Template for a N-poles ladder style filter based on TPT
 * design.
 *
 * @note Reference: Chapter 5.3 of "The art of VA filter design"
 * by Vadim Zavalishin.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 * @tparam N_FILTERS Number of one pole filters in the series.
 * @tparam ONE_POLE_FILTER Type of the one pole filter to use.
 */
template <uint32_t SAMPLE_RATE, uint32_t N_FILTERS, typename ONE_POLE_FILTER>
class VAResonantSeries : public AudioFilterBase<SAMPLE_RATE> {
 protected:
  std::array<ONE_POLE_FILTER, N_FILTERS> one_pole_filters;
  std::array<real_type, N_FILTERS> gain_powers;
  real_type max_resonance;
  real_type resonance;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(VAResonantSeries);

  virtual ~VAResonantSeries() = default;

  /**
   * @param cutoff_freq Cutoff frequency of the filter in Hz.
   * @param resonance Resonance of the filter (between 0 and 1).
   * @param max_resonance Maximum resonance value.
   */
  VAResonantSeries(real_type cutoff_freq, real_type resonance,
                   real_type max_resonance);

  inline void reset() noexcept override;

  inline real_type process_sample(real_type input) noexcept override;

  inline void set_freq(real_type cutoff_freq) noexcept override;

  inline void set_resonance(real_type resonance) noexcept override;

 protected:
  virtual real_type compute_series_input(real_type input) noexcept = 0;
};

// VA 1P =================================================================

/**
 * @brief One pole low pass IIR filter based on TPT discretization
 * (zero delay feedback design).
 *
 * @note Reference: Chapter 3.10 of "The art of VA filter design"
 * by Vadim Zavalishin.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
class VALowpass1P : public AudioFilterBase<SAMPLE_RATE> {
 protected:
  real_type s;  // State
  real_type g;  // Integrator gain
  real_type G;  // 1 / (1 + g)

 public:
  AT_DISABLE_COPY_AND_MOVE_M(VALowpass1P);

  virtual ~VALowpass1P() = default;

  /**
   * @brief Construct a new VALowpass1P object.
   *
   * @param cutoff_freq Cutoff frequency of the filter in Hz.
   */
  VALowpass1P(real_type cutoff_freq = 1000.0f);

  inline void reset() noexcept override;

  inline real_type process_sample(real_type input) noexcept override;

  void set_freq(real_type cutoff_freq) noexcept override;

  inline void set_resonance(
      [[maybe_unused]] real_type resonance) noexcept override {}

  /**
   * @brief Get G.
   *
   * @return G = g / (1 + g).
   */
  inline real_type get_instantaneuous_gain() const noexcept;

  /**
   * @brief Get S.
   *
   * @return S = state / (1 + g)
   */
  inline real_type get_instantaneuous_bias() const noexcept;
};

/**
 * @brief One pole high pass IIR filter based on TPT discretization
 * (zero delay feedback design).
 *
 * @note Reference: Chapter 3.10 of "The art of VA filter design"
 * by Vadim Zavalishin.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
class VAHighpass1P : public VALowpass1P<SAMPLE_RATE> {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(VAHighpass1P);

  virtual ~VAHighpass1P() = default;

  /**
   * @brief Construct a new VAHighpass1P object.
   *
   * @param cutoff_freq Cutoff frequency of the filter in Hz.
   */
  VAHighpass1P(real_type cutoff_freq = 1000.0f);

  inline real_type process_sample(real_type input) noexcept override;

  /**
   * @brief Get G.
   *
   * @return G = 1 / (1 + g).
   */
  inline real_type get_instantaneuous_gain() const noexcept;

  /**
   * @brief Get S.
   *
   * @return S = - state / (1 + g)
   */
  inline real_type get_instantaneuous_bias() const noexcept;
};

// VA 2P =================================================================

/**
 * @brief 2-poles ladder IIR filter based on TPT discretization
 * (zero delay feedback design).
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 * @tparam FILTER Type of the one pole filter to use.
 */
template <uint32_t SAMPLE_RATE, typename FILTER>
class VALadder2P : public VAResonantSeries<SAMPLE_RATE, 2, FILTER> {
  static constexpr real_type VA_LADDER_2P_MAX_RESONANCE = 4.0f;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(VALadder2P);

  virtual ~VALadder2P() = default;

  VALadder2P(real_type cutoff_freq = 1000.0f, real_type resonance = 0,
             real_type max_resonance = VA_LADDER_2P_MAX_RESONANCE);

 protected:
  real_type compute_series_input(real_type input) noexcept override;
};

/**
 * @brief Two poles ladder low pass IIR filter based on TPT discretization.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
using VALowpassLadder2P = VALadder2P<SAMPLE_RATE, VALowpass1P<SAMPLE_RATE>>;

/**
 * @brief Two poles ladder high pass IIR filter based on TPT discretization.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
using VAHighpassLadder2P = VALadder2P<SAMPLE_RATE, VAHighpass1P<SAMPLE_RATE>>;

// VA 4P =================================================================

/**
 * @brief 4-poles ladder IIR filter based on TPT discretization
 * (zero delay feedback design).
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 * @tparam FILTER Type of the one pole filter to use.
 */
template <uint32_t SAMPLE_RATE, typename FILTER>
class VALadder4P : public VAResonantSeries<SAMPLE_RATE, 4, FILTER> {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(VALadder4P);

  virtual ~VALadder4P() = default;

  VALadder4P(real_type cutoff_freq = 1000.0f, real_type resonance = 0);

 protected:
  real_type compute_series_input(real_type input) noexcept override;
};

/**
 * @brief Four poles ladder low pass IIR filter based on TPT discretization.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
using VALowpassLadder4P = VALadder4P<SAMPLE_RATE, VALowpass1P<SAMPLE_RATE>>;

/**
 * @brief Four poles ladder high pass IIR filter based on TPT discretization.
 *
 * @tparam SAMPLE_RATE Sample rate of the filter in Hz.
 */
template <uint32_t SAMPLE_RATE>
using VAHighpassLadder4P = VALadder4P<SAMPLE_RATE, VAHighpass1P<SAMPLE_RATE>>;

}  // namespace audio_toolbox

#include "audio_toolbox/processors/virtual_analog_filters_imp.hpp"

#endif  // _AUDIO_TOOLBOX_VIRTUAL_ANALOG_FILTERS_1P_H_