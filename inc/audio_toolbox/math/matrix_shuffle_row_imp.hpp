#include "matrix_shuffle_row.hpp"

namespace audio_toolbox
{

template <typename T>
ShuffleRowMatrix<T>::ShuffleRowMatrix(uint32_t rows, uint32_t cols)
    : matrix(rows)
{
    for (uint32_t i = 0; i < rows; i++)
    {
        matrix[i] = std::vector<T>(cols, static_cast<T>(0));
    }
}

template <typename T>
void ShuffleRowMatrix<T>::permute_rows(const std::vector<uint32_t>& permutation)
{
    if (permutation.size() != matrix.size())
    {
        throw std::invalid_argument("Permutation vector size must match matrix row count.");
    }

    std::vector<std::vector<T>> new_matrix;
    new_matrix.reserve(matrix.size());

    for (uint32_t idx : permutation)
    {
        // Move the vector directly into the new matrix
        new_matrix.push_back(std::move(matrix[idx]));
    }

    matrix = std::move(new_matrix);
}

} // namespace audio_toolbox
