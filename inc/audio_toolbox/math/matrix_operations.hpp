#pragma once

#include <concepts>
#include <stdexcept>
#include <type_traits>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"

namespace audio_toolbox
{

/*
 * We provide a common interface for MATRIX
 * and VECTOR that can be used in the matrix operations:
 *
 * VECTOR interface:
 *    size_t size() const;
 *    T operator[](uint32_t i) const;
 *
 * MATRIX interface:
 *    uint32_t get_num_rows() const;
 *    uint32_t get_num_columns() const;
 *    VECTOR operator[](uint32_t i); // Returns the i-th row of the matrix
 *
 * MATRIX can also define the following method to provide
 * an optimized implementation of the products:
 *     void dot( VECTOR& input, VECTOR& output) ;
 *     void dot( MATRIX& input, MATRIX& output) ;
 */

/**
 * @brief Computes the product of a matrix and a vector.
 *
 * @tparam VECTOR Vector type.
 * @tparam MATRIX Matrix type.
 * @param matrix Matrix (rows, cols).
 * @param vector Vector (cols).
 * @param result Result vector (rows).
 */
template <typename VECTOR, typename MATRIX>
void compute_matrix_vector_product(MATRIX& matrix, VECTOR& vector, VECTOR& result);

/**
 * @brief Computes the product of two matrices.
 *
 * @tparam T Element type.
 * @tparam MAT_A First matrix type.
 * @tparam MAT_B Second matrix type.
 * @tparam MAT_C Result matrix type.
 * @param matrix_a First matrix (NA, M).
 * @param matrix_b Second matrix (M, NB).
 * @param result Result matrix (NA, NB).
 */
template <typename T, typename MAT_A, typename MAT_B, typename MAT_C>
void compute_matrix_product(MAT_A& matrix_a, MAT_B& matrix_b, MAT_C& result);

} // namespace audio_toolbox

#include "audio_toolbox/math/matrix_operations_imp.hpp"
