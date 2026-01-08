#include "audio_toolbox/processors/bitcrush.hpp"

#include "audio_toolbox/math/random.hpp"
#include "test_utilities.hpp"

const uint32_t SAMPLE_RATE = 16000;
const uint32_t BUFFER_SIZE = SAMPLE_RATE / 2;

using namespace audio_toolbox;
using Buffer = AudioBuffer<BUFFER_SIZE>;

class BitCrushTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    auto* x = input_buffer.get_write_ptr();
    auto* y = output_buffer.get_write_ptr();
    noise_gen.process(x, input_buffer.size());
    bitcrush.process(x, y, input_buffer.size());
  }

  void TearDown() override {}

  BitCrush bitcrush;
  Buffer input_buffer;
  Buffer output_buffer;
  WhiteNoiseGen noise_gen;
};

TEST_F(BitCrushTest, IsStable) {
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(output_buffer[i], 1.e10f);
  }
}

TEST_F(BitCrushTest, OutputNotZeros) {
  EXPECT_GT(compute_max(output_buffer.get_read_ptr(), output_buffer.size()),
            1e-5f);
}