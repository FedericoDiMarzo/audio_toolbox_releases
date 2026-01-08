

namespace audio_toolbox {

#if AT_ENABLE_EFFICIENT_RAND == 0

template <uint32_t N>
std::array<uint32_t, N> compute_random_permutation() {
  auto seq = compute_int_range<N>(0);
  auto& gen = get_random_generator();
  std::shuffle(seq.begin(), seq.end(), gen);
  return seq;
}

inline void WhiteNoiseGen::process(real_type* output, uint32_t len,
                                   real_type std) noexcept {
  auto& gen = get_random_generator();
  for (uint32_t i = 0; i < len; i++) {
    output[i] = distribution(gen) * std;
  }
}

#else

template <uint32_t N>
std::array<uint32_t, N> compute_random_permutation() {
  auto seq = compute_int_range<N>(0);
  for (uint32_t i = 0; i < N; ++i) {
    uint32_t j = get_random_int(0, N - 1);
    std::swap(seq[i], seq[j]);
  }
  return seq;
}

inline void WhiteNoiseGen::process(real_type* output, uint32_t len,
                                   real_type std) noexcept {
  for (uint32_t i = 0; i < len; i++) {
    output[i] = get_random_real(-1.0f, 1.0f) * std;
  }
}

#endif

}  // namespace audio_toolbox
