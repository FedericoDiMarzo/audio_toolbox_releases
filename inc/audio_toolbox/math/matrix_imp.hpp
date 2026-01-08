
namespace audio_toolbox {

template <typename T, typename MAT>
void compute_matrix_vector_product(MAT& matrix, const T* vector, T* result) {
  for (uint32_t i = 0; i < matrix.get_rows_count(); i++) {
    result[i] = 0.0f;
    for (uint32_t j = 0; j < matrix.get_cols_count(); j++) {
      result[i] += matrix[i][j] * vector[j];
    }
  }
}

template <typename T, typename MAT_A, typename MAT_B, typename MAT_C>
void compute_matrix_product(MAT_A& matrix_a, MAT_B& matrix_b, MAT_C& result) {
  for (uint32_t i = 0; i < matrix_a.get_rows_count(); i++) {
    for (uint32_t j = 0; j < matrix_b.get_cols_count(); j++) {
      result[i][j] = 0.0f;
      for (uint32_t k = 0; k < matrix_a.get_cols_count(); k++) {
        result[i][j] += matrix_a[i][k] * matrix_b[k][j];
      }
    }
  }
}

_ROW_SHUFFLE_MATRIX_TEMPLATE
_ROW_SHUFFLE_MATRIX_TYPE::RowShuffleMatrix() {
  for (uint32_t i = 0; i < ROWS; i++) {
    matrix[i] = std::make_unique<std::array<T, COLS>>();
  }
}

_ROW_SHUFFLE_MATRIX_TEMPLATE
inline uint32_t _ROW_SHUFFLE_MATRIX_TYPE::get_rows_count() const {
  return ROWS;
}

_ROW_SHUFFLE_MATRIX_TEMPLATE
inline uint32_t _ROW_SHUFFLE_MATRIX_TYPE::get_cols_count() const {
  return COLS;
}

_ROW_SHUFFLE_MATRIX_TEMPLATE
std::array<T, COLS>& _ROW_SHUFFLE_MATRIX_TYPE::operator[](uint32_t i) {
  return *matrix[i];
}

_ROW_SHUFFLE_MATRIX_TEMPLATE
void _ROW_SHUFFLE_MATRIX_TYPE::permute_rows(
    const std::array<uint32_t, ROWS>& permutation) {
  std::array<std::unique_ptr<std::array<T, COLS>>, ROWS> new_matrix;
  for (uint32_t i = 0; i < ROWS; i++) {
    new_matrix[i] = std::make_unique<std::array<T, COLS>>();
    for (uint32_t j = 0; j < COLS; j++) {
      (*new_matrix[i])[j] = (*matrix[permutation[i]])[j];
    }
  }
  matrix = std::move(new_matrix);
}

_HADAMARD_MATRIX_TEMPLATE
_HADAMARD_MATRIX_TYPE::HadamardMatrix() { randomize(); }

_HADAMARD_MATRIX_TEMPLATE
void _HADAMARD_MATRIX_TYPE::dot(const real_type* input,
                                real_type* output) noexcept {
  dot_recursive(input, output, N);
  apply_permutation(output);
  if (NORMALIZED) normalize(output);
}
_HADAMARD_MATRIX_TEMPLATE
void _HADAMARD_MATRIX_TYPE::randomize() {
  permutation = compute_random_permutation<N>();
}

_HADAMARD_MATRIX_TEMPLATE
void _HADAMARD_MATRIX_TYPE::normalize(real_type* output) noexcept {
  static const real_type scale = 1.0f / std::sqrt(static_cast<real_type>(N));
  for (uint32_t i = 0; i < N; i++) {
    output[i] *= scale;
  }
}

_HADAMARD_MATRIX_TEMPLATE
void _HADAMARD_MATRIX_TYPE::dot_recursive(const real_type* input,
                                          real_type* output,
                                          uint32_t n) noexcept {
  // Base case
  if (n <= 1) {
    output[0] = input[0];
    return;
  }

  // Recursion
  uint32_t n_half = n / 2;
  dot_recursive(input, output, n_half);
  dot_recursive(input + n_half, output + n_half, n_half);

  // Combine the two halves using sum/difference
  for (uint32_t i = 0; i < n_half; ++i) {
    real_type a = output[i];
    real_type b = output[i + n_half];
    output[i] = (a + b);
    output[i + n_half] = (a - b);
  }
}

_HADAMARD_MATRIX_TEMPLATE
void _HADAMARD_MATRIX_TYPE::apply_permutation(real_type* output) noexcept {
  for (uint32_t i = 0; i < N; i++) {
    tmp[i] = output[permutation[i]];
  }
  std::copy_n(tmp.begin(), N, output);
}

_HOUSEHOLDER_MATRIX_TEMPLATE
void _HOUSEHOLDER_MATRIX_TYPE::dot(const real_type* input, real_type* output) {
  real_type sum = 0;
  for (uint32_t i = 0; i < N; ++i) {
    sum += input[i];
  }

  sum *= multiplier;

  for (uint32_t i = 0; i < N; ++i) {
    output[i] = input[i] + sum;
  }
}

}  // namespace audio_toolbox
