#include "audio_toolbox/processors/feedback.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class FeedbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    auto* x = input_buffer.get_write_ptr();
    noise_gen.process(x, BUFFER_SIZE * CHANNELS);
  }

  void TearDown() override {}

  static constexpr uint32_t CHANNELS = 4;
  static constexpr uint32_t SAMPLE_RATE = 16000;
  static constexpr uint32_t BUFFER_SIZE = SAMPLE_RATE / 8;

  using Buffer = AudioBuffer<BUFFER_SIZE, CHANNELS>;

  Feedback<CHANNELS, 256> feedback;
  Buffer input_buffer;
  Buffer output_buffer;
  WhiteNoiseGen noise_gen;
};

TEST_F(FeedbackTest, IsStable) {
  const auto* x = input_buffer.get_read_ptr();
  auto* y = output_buffer.get_write_ptr();
  feedback.process(x, y, BUFFER_SIZE);
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(y[i], 1.e10f);
  }
}
