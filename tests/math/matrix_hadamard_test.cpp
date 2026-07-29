
#include "audio_toolbox/math/matrix_hadamard.hpp"
#include "audio_toolbox/math/matrix_operations.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class HadamardMatrixTest : public ::testing::Test
{
  protected:
    void SetUp() override {}

    void TearDown() override {}

    HadamardMatrix<real_type> had_mat_2{ 2 };
    HadamardMatrix<real_type> had_mat_4{ 4 };
};

TEST_F(HadamardMatrixTest, HadamardMatrix2)
{
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    // H2 = [1  1]
    //      [1 -1]
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    had_mat_2[0][0] = 1;
    had_mat_2[0][1] = 1;
    had_mat_2[1][0] = 1;
    had_mat_2[1][1] = -1;
}

TEST_F(HadamardMatrixTest, HadamardMatrix4)
{
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    // H4 = [H2  H2]
    //      [H2 -H2]
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    had_mat_4[0][0] = had_mat_4[2][0];
    had_mat_4[0][1] = had_mat_4[2][1];
    had_mat_4[1][0] = had_mat_4[3][0];
    had_mat_4[1][1] = had_mat_4[3][1];
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    had_mat_4[0][0] = had_mat_4[2][0];
    had_mat_4[0][1] = had_mat_4[2][1];
    had_mat_4[1][0] = had_mat_4[3][0];
    had_mat_4[1][1] = had_mat_4[3][1];
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    had_mat_4[0][0] = -had_mat_4[2][2];
    had_mat_4[0][1] = -had_mat_4[2][3];
    had_mat_4[1][0] = -had_mat_4[3][2];
    had_mat_4[1][1] = -had_mat_4[3][3];
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}

TEST_F(HadamardMatrixTest, DotVector)
{
    std::vector<real_type> input{ 1.0f, 2.0f };
    std::vector<real_type> output(2);
    std::vector<real_type> expected_output{ 3.0f, -1.0f };
    had_mat_2.dot(input.data(), output.data());
    expect_all_near(output.data(), expected_output.data(), output.size(), 1e-6f);
}

TEST_F(HadamardMatrixTest, DotMatrix) {}
