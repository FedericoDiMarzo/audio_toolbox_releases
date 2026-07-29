
#include "audio_toolbox/math/matrix_operations.hpp"
#include "audio_toolbox/math/matrix_shuffle_row.hpp"
#include "audio_toolbox/math/random.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class ShuffleRowMatrixTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        set_random_seed(1);

        // Compute a random permutation of the rows
        row_permutation = compute_random_permutation(matrix.get_num_rows());

        // Randomize the vector
        WhiteNoiseGen noise;
        noise.process(vector.data(), vector.size());

        // Randomize the matrix
        for (uint32_t i = 0; i < matrix.get_num_rows(); i++)
        {
            noise.process(matrix[i].data(), matrix.get_num_columns());
        }

        // Compute the shuffled matrix
        shuffled_matrix = matrix;
        shuffled_matrix.permute_rows(row_permutation);

        // Compute the matrix-vector products
        compute_matrix_vector_product(matrix, vector, result);
        compute_matrix_vector_product(shuffled_matrix, vector, shuffled_result);
    }

    void TearDown() override {}

    ShuffleRowMatrix<real_type> matrix{ 4, 5 };
    ShuffleRowMatrix<real_type> shuffled_matrix{ 4, 5 };
    std::vector<uint32_t> row_permutation;
    std::vector<real_type> vector{ 5 };
    std::vector<real_type> result{ 5 };
    std::vector<real_type> shuffled_result{ 5 };
};

TEST_F(ShuffleRowMatrixTest, ShuffleRowMatrixPermuterows)
{
    for (uint32_t i = 0; i < matrix.get_num_rows(); i++)
    {
        for (uint32_t j = 0; j < matrix.get_num_columns(); j++)
        {
            EXPECT_NEAR(matrix[row_permutation[i]][j], shuffled_matrix[i][j], 1e-6f);
        }
    }
}

TEST_F(ShuffleRowMatrixTest, ShuffleRowMatrixDotVector)
{
    for (uint32_t i = 0; i < result.size(); i++)
    {
        EXPECT_NEAR(result[row_permutation[i]], shuffled_result[i], 1e-6f);
    }
}

TEST_F(ShuffleRowMatrixTest, ShuffleRowMatrixDotMatrix)
{
    // We don't check the result here
    ShuffleRowMatrix<real_type> mat_t(matrix.get_num_columns(), matrix.get_num_rows());
    ShuffleRowMatrix<real_type> result(matrix.get_num_rows(), matrix.get_num_rows());
    compute_matrix_product<real_type>(matrix, mat_t, result);
}
