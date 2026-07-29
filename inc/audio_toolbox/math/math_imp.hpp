
namespace audio_toolbox
{

template <typename T>
void apply_gain(T* x, uint32_t n, real_type gain) noexcept
{
    std::transform(x, x + n, x, [gain](T x) { return x * gain; });
}

template <typename T>
void apply_gain_db(T* x, uint32_t n, real_type gain_db) noexcept
{
    real_type gain = compute_inverse_db(gain_db);
    apply_gain(x, n, gain);
}

template <typename T>
void compute_abs(const T* x, T* y, uint32_t n) noexcept
{
    std::transform(x, x + n, y, [](T x) { return std::abs(x); });
}

template <typename T>
real_type compute_max(T* x, uint32_t n) noexcept
{
    auto itr = std::max_element(x, x + n);
    // Empty array
    if (itr == x + n)
        return 0;
    // Non-empty array
    return *itr;
}

template <typename T>
real_type compute_min(T* x, uint32_t n) noexcept
{
    auto itr = std::min_element(x, x + n);
    // Empty array
    if (itr == x + n)
        return 0;
    // Non-empty array
    return *itr;
}

template <typename T>
void apply_peak_normalization(const T* x, T* y, uint32_t n, real_type peak) noexcept
{
    compute_abs(x, y, n);
    real_type max_val = compute_max(y, n);
    real_type gain = peak / max_val;
    std::transform(x, x + n, y, [gain](T x) { return x * gain; });
}

template <typename T>
std::vector<T> create_int_range(T start, T end)
{
    std::vector<T> range(end - start);
    std::iota(range.begin(), range.end(), start);
    return range;
}

} // namespace audio_toolbox
