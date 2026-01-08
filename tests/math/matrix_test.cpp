
#include "audio_toolbox/math/matrix.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

const uint32_t ROWS = 3;
const uint32_t COLS = 8;

class MatrixTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    row_permutation = compute_random_permutation<ROWS>();

    for (uint32_t i = 0; i < ROWS; i++) {
      for (uint32_t j = 0; j < COLS; j++) {
        for (uint32_t k = 0; k < row_shf_mats.size(); k++) {
          row_shf_mats[k][i][j] = i * COLS + j;
        }
      }
    }
  }

  void TearDown() override {}

  using RSM = RowShuffleMatrix<real_type, ROWS, COLS>;
  using HM = HadamardMatrix<COLS>;

  std::array<RSM, 2> row_shf_mats = {RSM{}, RSM{}};
  std::array<HM, 4> hadamard_mats = {HM{}, HM{}, HM{}, HM{}};
  std::array<uint32_t, ROWS> row_permutation;
  std::array<std::array<real_type, COLS>, 2> vectors_col;
  std::array<std::array<real_type, COLS>, 2> vectors_row;
};

TEST_F(MatrixTest, RowShuffleMatrixPermuteRows) {
  row_shf_mats[0].permute_rows(row_permutation);
  for (uint32_t i = 0; i < ROWS; i++) {
    for (uint32_t j = 0; j < COLS; j++) {
      EXPECT_NEAR(row_shf_mats[0][i][j], row_shf_mats[1][row_permutation[i]][j],
                  1e-6f);
    }
  }
}

TEST_F(MatrixTest, RowShuffleMatrixDotVector) {
  compute_matrix_vector_product(row_shf_mats[0], vectors_col[0].data(),
                                vectors_row[0].data());
  auto zeros = get_zeros<real_type, ROWS>();
  expect_all_near(vectors_row[0].data(), zeros.data(), ROWS);
}

TEST_F(MatrixTest, RowShuffleMatrixDotMatrix) {
  using mat_type_1 = RowShuffleMatrix<real_type, COLS, ROWS>;
  using mat_type_2 = RowShuffleMatrix<real_type, ROWS, COLS>;
  mat_type_1 mat_t;
  mat_type_2 result;
  compute_matrix_product<real_type>(row_shf_mats[0], mat_t, result);
  // We don't check the result here
}
