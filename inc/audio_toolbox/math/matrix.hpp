#ifndef _AUDIO_TOOLBOX_MATRIX_H_
#define _AUDIO_TOOLBOX_MATRIX_H_

#include <array>
#include <cstdint>
#include <memory>
#include <numeric>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/random.hpp"

#define _ROW_SHUFFLE_MATRIX_TYPE RowShuffleMatrix<T, ROWS, COLS>
#define _ROW_SHUFFLE_MATRIX_TEMPLATE \
  template <typename T, uint32_t ROWS, uint32_t COLS>

#define _HADAMARD_MATRIX_TYPE HadamardMatrix<N, NORMALIZED>
#define _HADAMARD_MATRIX_TEMPLATE template <uint32_t N, bool NORMALIZED>

#define _HOUSEHOLDER_MATRIX_TYPE HouseholderMatrix<N>
#define _HOUSEHOLDER_MATRIX_TEMPLATE template <uint32_t N>

namespace audio_toolbox {

/**
 * @brief Computes the product of a matrix and a vector.
 *
 * @tparam T Vector type.
 * @tparam MAT Matrix type.
 * @param matrix Matrix (rows, cols).
 * @param vector Vector (cols).
 * @param result Result vector (rows).
 */
template <typename T, typename MAT>
void compute_matrix_vector_product(MAT& matrix, const T* vector, T* result);

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

/**
 * @brief Matrix class that allows permutation of rows.
 *
 * @tparam T Type of the matrix elements.
 * @tparam ROWS Number of rows in the matrix.
 * @tparam COLS Number of columns in the matrix.
 */
template <typename T, uint32_t ROWS, uint32_t COLS>
class RowShuffleMatrix {
  std::array<std::unique_ptr<std::array<T, COLS>>, ROWS> matrix;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(RowShuffleMatrix);

  RowShuffleMatrix();

  /**
   * @brief Gets the number of rows in the matrix.
   *
   * @return Number of rows.
   */
  inline uint32_t get_rows_count() const;

  /**
   * @brief Gets the number of columns in the matrix.
   *
   * @return Number of columns.
   */
  inline uint32_t get_cols_count() const;

  /**
   * @brief Gets a row of the matrix.
   *
   * @param i Element index.
   * @return Row of the matrix.
   */
  std::array<T, COLS>& operator[](uint32_t i);

  /**
   * @brief Permute the rows of the matrix.
   *
   * @param permutation Sequence of row indices.
   */
  void permute_rows(const std::array<uint32_t, ROWS>& permutation);
};

/**
 * @brief Hadamard matrix class. This matrix is an orthogonal matrix with
 * elements of +1 and -1.
 *
 * @tparam N Size of the Hadamard matrix (must be a power of two).
 * @tparam NORMALIZED If true, the matrix is normalized.
 */
template <uint32_t N, bool NORMALIZED = false>
class HadamardMatrix {
  static_assert(is_power_of_two(N), "N must be a power of two");

  std::array<uint32_t, N> permutation;
  std::array<real_type, N> tmp;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(HadamardMatrix);

  HadamardMatrix();

  /**
   * @brief Dot product of the input vector with the Hadamard matrix.
   *
   * @param input Input vector (N).
   * @param output Output vector (N).
   */
  void dot(const real_type* input, real_type* output) noexcept;

 private:
  /**
   * @brief Randomize the output permutation.
   */
  void randomize();

  /**
   * @brief Apply normalization to the output.
   *
   * @param output Output vector (N).
   */
  void normalize(real_type* output) noexcept;

  void dot_recursive(const real_type* input, real_type* output,
                     uint32_t n) noexcept;

  /**
   * @brief Apply the permutation to the output.
   *
   * @param output Output vector (N).
   */
  void apply_permutation(real_type* output) noexcept;
};

/**
 * @brief Householder matrix class. This matrix is an orthogonal matrix that
 * reflects a vector about a plane.
 *
 * https://en.wikipedia.org/wiki/Householder_transformation
 *
 * @tparam N Size of the Householder matrix.
 */
template <uint32_t N>
class HouseholderMatrix {
  static_assert(N >= 1, "N must be greater than or equal to 1");

  static constexpr real_type multiplier{-2.0f / N};

 public:
  static void dot(const real_type* input, real_type* output);
};

}  // namespace audio_toolbox

#include "audio_toolbox/math/matrix_imp.hpp"

#endif  // _AUDIO_TOOLBOX_MATRIX_H_