

namespace audio_toolbox {

_BIT_CRASTOR_TEMPLATE
_BIT_CRASTOR_TYPE::BitCrastor() {
  set_bitcrush_amount(0.0f);
  reset();
}

_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::reset() noexcept {
  for (auto& filter : filters) {
    filter->reset();
  }
  bitcrusher.reset();
}

_BIT_CRASTOR_TEMPLATE
real_type _BIT_CRASTOR_TYPE::process_sample(real_type input) noexcept {
  // By-pass
  if (!enable_processing) {
    return input;
  }

  // Effect on
  real_type y;
  // Order of processing
  if (bitcrusher_post_filter) {
    y = process_bitcrusher(process_filters(input));
  } else {
    y = process_filters(process_bitcrusher(input));
  }
  return y * gain_on;
}

_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::set_frequency(real_type freq) noexcept {
  for (auto& filter : filters) {
    filter->set_freq(freq);
  }
}

_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::set_resonance(real_type res) noexcept {
  for (auto& filter : filters) {
    filter->set_resonance(res);
  }
}
_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::set_bitcrush_amount(real_type amount) noexcept {
  // A single parameter controls the bit depth, downsample factor, and
  // clip
  amount = clip(amount, 0.0f, 1.0f);
  uint8_t bit_depth = static_cast<uint8_t>(
      linear_interpolation(bitdepth_range[0], bitdepth_range[1], amount));
  uint32_t downsample_factor = static_cast<uint32_t>(
      linear_interpolation(downsample_range[0], downsample_range[1], amount));
  real_type clip_threshold =
      linear_interpolation(cb_clip_range[0], cb_clip_range[1], amount);

  // Setting the parameters
  bitcrusher.set_bit_depth(bit_depth);
  bitcrusher.set_downsample_factor(downsample_factor);
  bitcrusher.set_clip_threshold(clip_threshold);
  gain = linear_interpolation(cb_gain_range[0], cb_gain_range[1], amount);
}
_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::toggle_processing() noexcept {
  enable_processing = !enable_processing;
}

_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::toggle_filters() noexcept {
  enable_filters = !enable_filters;
}
_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::toggle_filter_type() noexcept {
  enable_secondary_filter = !enable_secondary_filter;
}
_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::toggle_bitcrusher() noexcept {
  enable_bitcrusher = !enable_bitcrusher;
}
_BIT_CRASTOR_TEMPLATE
void _BIT_CRASTOR_TYPE::toggle_bitcrusher_post_filter() noexcept {
  bitcrusher_post_filter = !bitcrusher_post_filter;
}
_BIT_CRASTOR_TEMPLATE
bool _BIT_CRASTOR_TYPE::is_processing_enabled() const noexcept {
  return enable_processing;
}
_BIT_CRASTOR_TEMPLATE
bool _BIT_CRASTOR_TYPE::is_bitcrusher_enabled() const noexcept {
  return enable_bitcrusher;
}
_BIT_CRASTOR_TEMPLATE
bool _BIT_CRASTOR_TYPE::is_bitcrusher_post_filter() const noexcept {
  return bitcrusher_post_filter;
}
_BIT_CRASTOR_TEMPLATE
bool _BIT_CRASTOR_TYPE::are_filters_enabled() const noexcept {
  return enable_filters;
}
_BIT_CRASTOR_TEMPLATE
bool _BIT_CRASTOR_TYPE::is_secondary_filter_enabled() const noexcept {
  return enable_secondary_filter;
}
_BIT_CRASTOR_TEMPLATE
real_type _BIT_CRASTOR_TYPE::process_filters(real_type input) noexcept {
  if (enable_filters) {
    auto& f = filters[enable_secondary_filter];
    return f->process_sample(input);
  } else {
    return input;
  }
}

_BIT_CRASTOR_TEMPLATE
real_type _BIT_CRASTOR_TYPE::process_bitcrusher(real_type input) noexcept {
  if (enable_bitcrusher) {
    return bitcrusher.process_sample(input) * gain;
  } else {
    return input;
  }
}

}  // namespace audio_toolbox
