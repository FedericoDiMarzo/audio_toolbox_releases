

namespace audio_toolbox {

/**
 * @brief Use this function to clamp a delay value between 0 and max_delay - 1.
 *
 * @param delay Delay value.
 * @param max_delay Maximum delay value (plus one).
 * @return Clamped delay value.
 */
real_type clamp_delay(real_type delay, real_type max_delay) noexcept {
  return clip(delay, 0.0f, max_delay - 1.0f);
}

_INTEGER_DELAY_TEMPLATE
_INTEGER_DELAY_TYPE::IntegerDelay(uint32_t delay) {
  set_delay(delay);
  reset();
};

_INTEGER_DELAY_TEMPLATE
_INTEGER_DELAY_TYPE::IntegerDelay() {
  randomize();
  reset();
}

_INTEGER_DELAY_TEMPLATE
void _INTEGER_DELAY_TYPE::reset() noexcept { buffer.fill(0.0f); }

_INTEGER_DELAY_TEMPLATE
real_type _INTEGER_DELAY_TYPE::process_sample(real_type input) noexcept {
  buffer.push(input);
  return buffer[delay + 1];
}

_INTEGER_DELAY_TEMPLATE
CircularBuffer<MAX_DLY>& _INTEGER_DELAY_TYPE::get_buffer() noexcept {
  return buffer;
}

_INTEGER_DELAY_TEMPLATE
void _INTEGER_DELAY_TYPE::set_delay(uint32_t delay) noexcept {
  this->delay = clamp_delay(delay, MAX_DLY);
}

_INTEGER_DELAY_TEMPLATE
void _INTEGER_DELAY_TYPE::randomize() noexcept {
  set_delay(get_random_int(delay_min, delay_max));
}

_INTEGER_DELAY_TEMPLATE
void _INTEGER_DELAY_TYPE::set_random_range(real_type min,
                                           real_type max) noexcept {
  min = clamp_delay(min, MAX_DLY);
  max = clamp_delay(max, MAX_DLY);
  if (min > max) {
    std::swap(min, max);
  }
  delay_min = min;
  delay_max = max;
}

_INTEGER_DELAY_TEMPLATE
uint32_t _INTEGER_DELAY_TYPE::get_delay() const noexcept { return delay; }

_INTEGER_DELAY_TEMPLATE
_FRACTIONAL_DELAY_TYPE::FractionalDelay(real_type delay) {
  set_delay(delay);
  reset();
}

_INTEGER_DELAY_TEMPLATE
_FRACTIONAL_DELAY_TYPE::FractionalDelay() {
  randomize();
  reset();
}

_INTEGER_DELAY_TEMPLATE
void _FRACTIONAL_DELAY_TYPE::reset() noexcept { delay_line.reset(); }

_INTEGER_DELAY_TEMPLATE
real_type _FRACTIONAL_DELAY_TYPE::process_sample(real_type input) noexcept {
  auto& cb = delay_line.get_buffer();
  cb.push(input);
  real_type val1 = cb[delay_int + 1];
  real_type val2 = cb[delay_int + 2];
  return linear_interpolation(val1, val2, delay_dec);
}

_INTEGER_DELAY_TEMPLATE
real_type _FRACTIONAL_DELAY_TYPE::get_delay() const noexcept {
  return delay_int + delay_dec;
}

_INTEGER_DELAY_TEMPLATE
void _FRACTIONAL_DELAY_TYPE::randomize() noexcept {
  set_delay(get_random_real(static_cast<real_type>(delay_min),
                            static_cast<real_type>(delay_max)));
}

_INTEGER_DELAY_TEMPLATE
void _FRACTIONAL_DELAY_TYPE::set_random_range(real_type min,
                                              real_type max) noexcept {
  min = clamp_delay(min, MAX_DLY);
  max = clamp_delay(max, MAX_DLY);
  if (min > max) {
    std::swap(min, max);
  }
  delay_min = min;
  delay_max = max;
}

_FRACTIONAL_DELAY_TEMPLATE
void _FRACTIONAL_DELAY_TYPE::set_delay(real_type delay) noexcept {
  delay = clamp_delay(delay, MAX_DLY);
  delay_int = static_cast<uint32_t>(delay);
  delay_dec = delay - delay_int;
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
_MODULATED_FRACTIONAL_DELAY_TYPE::ModulatedFractionalDelay(real_type delay,
                                                           real_type frequency,
                                                           real_type depth) {
  set_delay(delay);
  set_frequency(frequency);
  set_depth(depth);
  reset();
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
_MODULATED_FRACTIONAL_DELAY_TYPE::ModulatedFractionalDelay()
    : ModulatedFractionalDelay(MAX_DLY / 2) {
  randomize();
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::reset() noexcept {
  delay_line.reset();
  lfo.reset();
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
real_type _MODULATED_FRACTIONAL_DELAY_TYPE::process_sample(
    real_type input) noexcept {
  real_type output = delay_line.process_sample(input);
  modulate_delay();
  return output;
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::set_delay(real_type delay) noexcept {
  delay_line.set_delay(delay);
  base_delay_amt = delay;
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::set_frequency(
    real_type frequency) noexcept {
  lfo.set_frequency(frequency);
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::set_phase(real_type phase) noexcept {
  lfo.set_phase(phase);
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::set_depth(real_type depth) noexcept {
  real_type dly = delay_line.get_delay();
  real_type depth_max = std::min(dly, MAX_DLY - 1 - dly);
  this->depth = clip(depth, 0.0f, depth_max);
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::set_random_range(
    real_type min, real_type max) noexcept {
  delay_line.set_random_range(min, max);
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::randomize() noexcept {
  delay_line.randomize();
  base_delay_amt = delay_line.get_delay();
}

_MODULATED_FRACTIONAL_DELAY_TEMPLATE
void _MODULATED_FRACTIONAL_DELAY_TYPE::modulate_delay() noexcept {
  real_type dly = base_delay_amt + depth * lfo.process_sample();
  delay_line.set_delay(dly);
}

};  // namespace audio_toolbox