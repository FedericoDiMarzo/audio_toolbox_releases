#include "audio_toolbox/combinators/bitcrastor.hpp"

#include "test_utilities.hpp"

const uint32_t SAMPLE_RATE = 16000;
const uint32_t BUFFER_SIZE = SAMPLE_RATE / 8;

using namespace audio_toolbox;
using Buffer = AudioBuffer<BUFFER_SIZE>;

class BitcrastorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    auto* x = input_buffer.get_write_ptr();
    auto* y = output_buffer.get_write_ptr();
    bitcrastor.toggle_bitcrusher();  // On
    bitcrastor.toggle_filters();     // On
    noise_gen.process(x, input_buffer.size());
    bitcrastor.process(x, y, input_buffer.size());
  }

  void TearDown() override {}

  BitCrastor<SAMPLE_RATE> bitcrastor;
  Buffer input_buffer;
  Buffer output_buffer;
  WhiteNoiseGen noise_gen;
};

TEST_F(BitcrastorTest, InitialFlags) {
  EXPECT_TRUE(bitcrastor.is_bitcrusher_enabled());  // Set before
  EXPECT_TRUE(bitcrastor.are_filters_enabled());    // Set before
  EXPECT_FALSE(bitcrastor.is_secondary_filter_enabled());
  EXPECT_FALSE(bitcrastor.is_bitcrusher_post_filter());
}

TEST_F(BitcrastorTest, IsStable) {
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(output_buffer[i], 1.e10f);
  }
}

TEST_F(BitcrastorTest, OutputNotZeros) {
  EXPECT_GT(compute_max(output_buffer.get_read_ptr(), output_buffer.size()),
            1e-5f);
}

// Checks that by changing the bitchrush amount the output volume
// does not change too much.
TEST_F(BitcrastorTest, BitCrushAmtIsBalanced) {
  Buffer output_buffer2;
  bitcrastor.set_bitcrush_amount(1.0f);
  bitcrastor.process(input_buffer.get_read_ptr(),
                     output_buffer2.get_write_ptr(), input_buffer.size());
  real_type energy1 =
      compute_energy(output_buffer.get_read_ptr(), output_buffer.size());
  real_type energy2 =
      compute_energy(output_buffer2.get_read_ptr(), output_buffer2.size());
  energy1 = compute_db(energy1);
  energy2 = compute_db(energy2);
  std::cout << "energy1: " << energy1 << std::endl;
  std::cout << "energy2: " << energy2 << std::endl;
  EXPECT_NEAR(energy1, energy2, 3.0f);
}