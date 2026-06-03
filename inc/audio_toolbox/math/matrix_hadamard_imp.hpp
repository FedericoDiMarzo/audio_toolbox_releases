#include "matrix_hadamard.hpp"

namespace audio_toolbox
{


template <typename T>
HadamardMatrix<T>::HadamardMatrix(uint32_t size, bool normalized)
    : tmp_buffer(size),
      normalized(normalized)
{
    if (!is_power_of_two(size))
    {
        throw std::invalid_argument("Hadamard matrix size must be a power of two.");
    }
    permutation = create_int_range<uint32_t>(0, size);
}

template <typename T>
void HadamardMatrix<T>::randomize_permutation()
{
    permutation = std::move(compute_random_permutation(get_num_rows()));
}

template <typename T>
inline void HadamardMatrix<T>::permutate_output(const T* input, T* output) noexcept
{
    for (uint32_t i = 0; i < get_num_rows(); i++)
    {
        output[i] = input[permutation[i]];
    }
}

template <typename T>
void HadamardMatrix<T>::dot(const T* input, T* output) noexcept

{
    uint32_t n = get_num_rows();


    std::copy(input, input + n, tmp_buffer.data());

    // We start from the smallest Hadamard matrix (size 1)
    // and build up to the full size (n) doubling the size at each step.
    // [1  1] * [x0]  = [x0 + x1]
    // [1 -1]   [x1]    [x0 - x1]
    for (uint32_t block_size = 1; block_size < n; block_size <<= 1)
    {
        for (uint32_t i = 0; i < n; i += 2 * block_size)
        {
            for (uint32_t j = 0; j < block_size; j++)
            {
                T u = tmp_buffer[i + j];
                T v = tmp_buffer[i + block_size + j];
                tmp_buffer[i + j] = u + v;
                tmp_buffer[i + block_size + j] = u - v;
            }
        }
    }

    // We apply the permutation to the output
    permutate_output(tmp_buffer.data(), output);

    // Optional normalization
    if (normalized)
    {
        T scale = static_cast<T>(1) / std::sqrt(static_cast<T>(n));
        apply_gain(output, n, scale);
    }
}

namespace
{
/**
 * @brief Counts the number of set bits (1s) in the binary representation of n.
 * (Brian Kernighan’s Algorithm).
 *
 * @param n The number to count set bits in.
 * @return uint32_t The number of set bits.
 */
uint32_t count_set_bits(uint32_t n)
{
    uint32_t count = 0;
    while (n > 0)
    {
        n &= (n - 1); // Clears the least significant set bit
        count++;
    }
    return count;
}
} // namespace

template <typename T>
std::vector<T> HadamardMatrix<T>::operator[](uint32_t i)
{
    uint32_t n = get_num_rows();
    std::vector<T> row(n);

    // The actual row index after your custom permutation
    uint32_t permuted_i = permutation[i];

    for (uint32_t j = 0; j < n; ++j)
    {
        // H(i, j) is 1 if parity is even, -1 if odd
        uint32_t parity = count_set_bits(permuted_i & j);
        row[j] = (parity % 2 == 0) ? static_cast<T>(1) : static_cast<T>(-1);
    }

    return row;
}


} // namespace audio_toolbox