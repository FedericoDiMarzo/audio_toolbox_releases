#include "audio_toolbox/processors/diffusion_blocks.hpp"

#include "test_utilities.hpp"

const uint32_t CH_A = 4;
const uint32_t CH_B = 2;
const uint32_t BUFFER_SIZE = 100;

using namespace audio_toolbox;
using MonoBuffer = AudioBuffer<BUFFER_SIZE, 1>;
using BufferA = AudioBuffer<BUFFER_SIZE, CH_A>;
using BufferB = AudioBuffer<BUFFER_SIZE, CH_B>;

class DiffusionBlocksTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    for (auto& buffer : buffers_mono) {
      noise_gen.process(buffer.get_write_ptr(), buffer.size());
    }
    for (auto& buffer : buffers_type_a) {
      noise_gen.process(buffer.get_write_ptr(), buffer.size());
    }
    for (auto& buffer : buffers_type_b) {
      noise_gen.process(buffer.get_write_ptr(), buffer.size());
    }
  }

  void TearDown() override {}

  WhiteNoiseGen noise_gen;
  ChannelExpander<CH_A> expander;
  MultiChannelExpander<CH_B, CH_A> mc_expander;
  ChannelReducerAvg<CH_A, CH_B> mixer;
  ChannelReducer<CH_A, CH_B> reducer;
  ChannelBlender<CH_A> blender;

  std::array<MonoBuffer, 2> buffers_mono;
  std::array<BufferA, 2> buffers_type_a;
  std::array<BufferB, 2> buffers_type_b;
};

TEST_F(DiffusionBlocksTest, ChannelExpanderProcess) {
  expander.process(buffers_mono[0].get_read_ptr(),
                   buffers_type_a[0].get_write_ptr(), buffers_mono[0].size());
  auto* y = buffers_type_a[0].get_read_ptr();
  for (uint32_t i = 0; i < buffers_mono[0].size(); i++) {
    // Create a vector of the same value repeated CH_A times
    std::vector<real_type> expected(CH_A, buffers_mono[0][i]);
    expect_all_near(y, expected.data(), CH_A);
    y += CH_A;
  }
}

TEST_F(DiffusionBlocksTest, MultiChannelExpanderProcess) {
  std::array<real_type, CH_B> x = {0.1f, 0.2f};
  std::array<real_type, CH_A> y;
  mc_expander.process_sample(x.data(), y.data());
  EXPECT_NEAR(y[0], 0.1f, 1e-6);
  EXPECT_NEAR(y[1], 0.1f, 1e-6);
  EXPECT_NEAR(y[2], 0.2f, 1e-6);
  EXPECT_NEAR(y[2], 0.2f, 1e-6);
}

TEST_F(DiffusionBlocksTest, ChannelReducerAvgProcess) {
  static_assert(CH_A == 4, "CH_A must be 4");
  static_assert(CH_B == 2, "CH_B must be 2");
  auto* x = buffers_type_a[0].get_read_ptr();
  auto* y = buffers_type_b[0].get_write_ptr();
  mixer.process(x, y, buffers_type_a[0].get_len());
  for (uint32_t i = 0; i < buffers_type_a[0].get_len(); i++) {
    real_type ch0 = (x[0] + x[1]) / 2;
    real_type ch1 = (x[2] + x[3]) / 2;
    EXPECT_NEAR(y[0], ch0, 1e-6);
    EXPECT_NEAR(y[1], ch1, 1e-6);
    x += CH_A;
    y += CH_B;
  }
}

TEST_F(DiffusionBlocksTest, ChannelReducerProcess) {
  static_assert(CH_A == 4, "CH_A must be 4");
  static_assert(CH_B == 2, "CH_B must be 2");
  auto* x = buffers_type_a[0].get_read_ptr();
  auto* y = buffers_type_b[0].get_write_ptr();
  reducer.process(x, y, buffers_type_a[0].get_len());
  for (uint32_t i = 0; i < buffers_type_a[0].get_len(); i++) {
    real_type ch0 = x[0];
    real_type ch1 = x[2];
    EXPECT_NEAR(y[0], ch0, 1e-6);
    EXPECT_NEAR(y[1], ch1, 1e-6);
    x += CH_A;
    y += CH_B;
  }
}

TEST_F(DiffusionBlocksTest, ChannelBlenderProcess) {
  auto* x = buffers_type_a[0].get_read_ptr();
  auto* y = buffers_type_a[1].get_write_ptr();
  blender.process(x, y, buffers_type_a[0].get_len());
}

TEST_F(DiffusionBlocksTest, BlenderProcess) {
  auto* x = buffers_type_a[0].get_read_ptr();
  auto* y = buffers_type_a[1].get_write_ptr();
  blender.process(x, y, buffers_type_a[0].get_len());
}