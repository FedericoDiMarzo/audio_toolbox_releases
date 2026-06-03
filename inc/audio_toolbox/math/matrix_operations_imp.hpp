
namespace audio_toolbox
{

// Matrix-Vector Product // == == == == == == == == == == == == == == == ==

// We select the optimized matrix-vector product if the matrix
// defines a dot method with the correct signature.
template <typename MATRIX, typename VECTOR>
concept HasOptimizedMatrixVectorDotProduct =
    requires(MATRIX& matrix, VECTOR& vector, VECTOR& result) { matrix.dot(vector, result); };

// Optimized version
template <typename VECTOR, typename MATRIX>
    requires HasOptimizedMatrixVectorDotProduct<MATRIX, VECTOR>
void compute_matrix_vector_product(MATRIX& matrix, VECTOR& vector, VECTOR& result)
{
    matrix.dot(vector, result);
}

// Fallback version
template <typename VECTOR, typename MATRIX>
    requires(!HasOptimizedMatrixVectorDotProduct<MATRIX, VECTOR>)
void compute_matrix_vector_product(MATRIX& matrix, VECTOR& vector, VECTOR& result)
{
    for (uint32_t i = 0; i < matrix.get_num_rows(); i++)
    {
        result[i] = 0.0f;
        for (uint32_t j = 0; j < matrix.get_num_columns(); j++)
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

// Matrix-Matrix Product // == == == == == == == == == == == == == == == ==

// We select the optimized matrix-matrix product if the matrix allows it.
template <typename MAT_A, typename MAT_B, typename MAT_C>
concept HasOptimizedMatrixMatrixDotProduct =
    requires(MAT_A& matrix, MAT_B& other, MAT_C& result) { matrix.dot(other, result); };

// Optimized version
template <typename MAT_A, typename MAT_B, typename MAT_C>
    requires HasOptimizedMatrixMatrixDotProduct<MAT_A, MAT_B, MAT_C>
void compute_matrix_product(MAT_A& matrix_a, MAT_B& matrix_b, MAT_C& result)
{
    matrix_a.dot(matrix_b, result);
}

// Fallback version
template <typename T, typename MAT_A, typename MAT_B, typename MAT_C>
void compute_matrix_product(MAT_A& matrix_a, MAT_B& matrix_b, MAT_C& result)
{
    if (matrix_a.get_num_columns() != matrix_b.get_num_rows())
    {
        throw std::invalid_argument("Incompatible matrix dimensions for multiplication.");
    }
    if (matrix_a.get_num_rows() != result.get_num_rows() || matrix_b.get_num_columns() != result.get_num_columns())
    {
        throw std::invalid_argument("Result matrix has incorrect dimensions.");
    }
    for (uint32_t i = 0; i < matrix_a.get_num_rows(); i++)
    {
        for (uint32_t j = 0; j < matrix_b.get_num_columns(); j++)
        {
            result[i][j] = 0.0f;
            for (uint32_t k = 0; k < matrix_a.get_num_columns(); k++)
            {
                result[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
}


} // namespace audio_toolbox
