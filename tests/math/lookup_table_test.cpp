#include "audio_toolbox/math/lookup_table.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

const uint32_t TABLE_LEN = 10;

class LookupTableTest : public ::testing::Test {
 protected:
  void SetUp() override { set_random_seed(1); }

  void TearDown() override {}

  LookupTable<TABLE_LEN> lookup_table{[](float x) { return x; }, 0.0f, 9.0f};
};

TEST_F(LookupTableTest, GetExactValue) {
  real_type y = lookup_table(5.0f);
  EXPECT_FLOAT_EQ(y, 5.0f);
}

TEST_F(LookupTableTest, GetExactValueBoundary) {
  real_type y = lookup_table(0.0f);
  EXPECT_FLOAT_EQ(y, 0.0f);
  y = lookup_table(9.0f);
  EXPECT_FLOAT_EQ(y, 9.0f);
}

TEST_F(LookupTableTest, GetExactValueOutOfBoundary) {
  real_type y = lookup_table(-1.0f);
  EXPECT_FLOAT_EQ(y, 0.0f);
  y = lookup_table(10.0f);
  EXPECT_FLOAT_EQ(y, 9.0f);
}

TEST_F(LookupTableTest, GetInterpolatedValue) {
  real_type y = lookup_table(5.4f);
  EXPECT_FLOAT_EQ(y, 5.4f);
}