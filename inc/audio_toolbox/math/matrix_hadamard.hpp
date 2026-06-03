#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/static/constexpr.hpp"

// TODO: More testing
namespace audio_toolbox
{

/**
 * @brief Hadamard matrix class. This matrix is an orthogonal matrix with
 * elements of +1 and -1 (int). The size of the matrix must be a power of two.
 *
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class HadamardMatrix
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==


    std::vector<uint32_t> permutation;
    std::vector<T> tmp_buffer;
    bool normalized;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    AT_DISABLE_COPY_AND_MOVE_M(HadamardMatrix);

    /**
     * @param size Size of the square matrix (must be a power of two).
     * @param normalized If true, the matrix is normalized.
     */
    HadamardMatrix(uint32_t size, bool normalized = false);

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==

    inline uint32_t get_num_rows() const
    {
        return static_cast<uint32_t>(permutation.size());
    }

    inline uint32_t get_num_columns() const
    {
        return get_num_rows();
    }

    /**
     * @brief This method will create the i-th row of the Hadamard matrix on-the-fly.
     */
    std::vector<T> operator[](uint32_t i);

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Optimized dot product of the Hadamard matrix with an input vector.
     *
     * Fast Walsh-Hadamard Transform (FWHT)
     * https://en.wikipedia.org/wiki/Fast_Walsh%E2%80%93Hadamard_transform
     */
    void dot(const T* vector, T* output) noexcept;

    /**
     * @brief Randomize the output permutation.
     */
    void randomize_permutation();

  private:
    /**
     * @brief Randomly permutate the output of the Hadamard transform.
     */
    void permutate_output(const T* input, T* output) noexcept;
};

} // namespace audio_toolbox

#include "audio_toolbox/math/matrix_hadamard_imp.hpp"