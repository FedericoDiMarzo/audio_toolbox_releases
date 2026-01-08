#include "audio_toolbox/processors/reverbs.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class ReverbsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    auto* x = input_buffer.get_write_ptr();
    noise_gen.process(x, input_buffer.size());
  }

  void TearDown() override {}

  static constexpr uint32_t OUT_CHANNELS = 2;
  static constexpr uint32_t DIFF_CHANNELS = 4;
  static constexpr uint32_t SAMPLE_RATE = 16000;
  static constexpr uint32_t BUFFER_SIZE = SAMPLE_RATE / 8;
  static constexpr uint32_t MAX_DLY = 256;
  static constexpr uint32_t FB_DLY = 64;

  using RevDiffuse = ReverbDiffuse<SAMPLE_RATE, 1, OUT_CHANNELS, DIFF_CHANNELS,
                                   MAX_DLY, FB_DLY>;

  RevDiffuse rev_diff;
  AudioBuffer<BUFFER_SIZE> input_buffer;
  AudioBuffer<BUFFER_SIZE, OUT_CHANNELS> output_buffer;
  WhiteNoiseGen noise_gen;
};

TEST_F(ReverbsTest, IsStable) {
  const auto* x = input_buffer.get_read_ptr();
  auto* y = output_buffer.get_write_ptr();
  rev_diff.process(x, y, BUFFER_SIZE);
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(y[i], 1.e10f);
  }
}
