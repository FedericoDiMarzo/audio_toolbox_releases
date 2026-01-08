

namespace audio_toolbox {

_LOOKUP_TABLE_TEMPLATE
_LOOKUP_TABLE_TYPE::LookupTable(std::function<float(float)> function,
                                real_type arg_min, real_type arg_max)
    : arg_min(arg_min), arg_max(arg_max), arg_range(arg_max - arg_min) {
  real_type step = (arg_max - arg_min) / (SIZE - 1);
  for (unsigned int i = 0; i < SIZE; i++) {
    table[i] = function(arg_min + i * step);
  }
}

_LOOKUP_TABLE_TEMPLATE
real_type _LOOKUP_TABLE_TYPE::operator()(real_type x) const noexcept {
  if (x <= arg_min) {
    return table[0];
  } else if (x >= arg_max) {
    return table[SIZE - 1];
  } else {
    real_type idx = (x - arg_min) / arg_range * (SIZE - 1);
    unsigned int i = static_cast<uint32_t>(idx);
    return linear_interpolation(table[i], table[i + 1], idx - i);
  }
}

_LOOKUP_TABLE_TEMPLATE
size_t _LOOKUP_TABLE_TYPE::size() const noexcept { return SIZE; }

_LOOKUP_TABLE_TEMPLATE
real_type _LOOKUP_TABLE_TYPE::get_arg_min() const noexcept { return arg_min; }

_LOOKUP_TABLE_TEMPLATE
real_type _LOOKUP_TABLE_TYPE::get_arg_max() const noexcept { return arg_max; }

_LOOKUP_TABLE_TEMPLATE
real_type _LOOKUP_TABLE_TYPE::get_arg_range() const noexcept {
  return arg_range;
}

}  // namespace audio_toolbox
