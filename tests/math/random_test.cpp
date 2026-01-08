#include "test_utilities.hpp"

using namespace audio_toolbox;

#if AT_ENABLE_EFFICIENT_RAND == 0
class RandomTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  uint64_t seed = 0;
  std::default_random_engine& gen = get_random_generator();
};

TEST_F(RandomTest, SetRandomSeed) {
  set_random_seed(seed);
  int x = gen();
  set_random_seed(seed);
  int y = gen();
  EXPECT_EQ(x, y);
}

#else
class RandomTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  uint64_t seed = 10;
};

TEST_F(RandomTest, SetRandomSeed) {
  set_random_seed(seed);
  int x = get_random_int(0, 100);
  set_random_seed(seed);
  int y = get_random_int(0, 100);
  EXPECT_EQ(x, y);
}

#endif  // AT_ENABLE_EFFICIENT_RAND