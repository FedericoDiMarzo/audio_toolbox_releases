#pragma once

#include <cstdint>
#include <memory>
#include <numeric>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"

namespace audio_toolbox
{

/**
 * @brief Matrix class that allows permutation of rows.
 *
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class ShuffleRowMatrix
{
    // Attributes // == == == == == == == == == == == == == == == == == == == ==

    std::vector<std::vector<T>> matrix;

    // Construction // == == == == == == == == == == == == == == == == == == == ==

  public:
    ShuffleRowMatrix(ShuffleRowMatrix&&) = default;
    ShuffleRowMatrix& operator=(ShuffleRowMatrix&&) = default;
    ShuffleRowMatrix(const ShuffleRowMatrix&) = default;
    ShuffleRowMatrix& operator=(const ShuffleRowMatrix&) = default;

    /**
     * @brief Initializes the matrix with zeros.
     *
     * @param rows Number of rows in the matrix.
     * @param cols Number of columns in the matrix.
     */
    ShuffleRowMatrix(uint32_t rows, uint32_t cols);

    // Getters/Setters // == == == == == == == == == == == == == == == == == == ==


    /**
     * @brief Gets the number of rows in the matrix.
     *
     * @return Number of rows.
     */
    inline uint32_t get_num_rows() const
    {
        return static_cast<uint32_t>(matrix.size());
    }

    /**
     * @brief Gets the number of columns in the matrix.
     *
     * @return Number of columns.
     */
    inline uint32_t get_num_columns() const
    {
        return matrix.empty() ? 0 : static_cast<uint32_t>(matrix[0].size());
    }

    /**
     * @brief Gets a row of the matrix.
     *
     * @param i Element index.
     * @return Row of the matrix.
     */
    inline std::vector<T>& operator[](uint32_t i)
    {
        return matrix[i];
    }

    std::vector<std::vector<T>>& data()
    {
        return matrix;
    }

    // Processing // == == == == == == == == == == == == == == == == == == == ==

    /**
     * @brief Permute the rows of the matrix.
     *
     * @param permutation Sequence of row indices.
     */
    void permute_rows(const std::vector<uint32_t>& permutation);
};


} // namespace audio_toolbox

#include "audio_toolbox/math/matrix_shuffle_row_imp.hpp"
