#include "audio_toolbox/processors/paulstretch.hpp"

#include <array>

#include "test_utilities.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 16000;
constexpr uint32_t BUFFER_SIZE = next_power_of_two(SAMPLE_RATE * 3);
constexpr std::array<uint32_t, 2> FFT_LEN_ARRAY = {1024, 2048};

using Buffer = AudioBuffer<BUFFER_SIZE>;

using PS = PaulStretch<SAMPLE_RATE, 2, FFT_LEN_ARRAY, BUFFER_SIZE>;

uint8_t external_memory[BUFFER_SIZE * sizeof(real_type)];

class PaulStretchTest : public ::testing::Test {
 protected:
  void SetUp() override {
    set_random_seed(1);
    auto* x = input_buffer.get_write_ptr();
    noise_gen.process(x, input_buffer.size());
  }

  void process_input(PS& ts) {
    ts.process(input_buffer.get_read_ptr(), output_buffer.get_write_ptr(),
               input_buffer.size());
  }

  void TearDown() override {}

  PS time_stretch;
  Buffer input_buffer;
  Buffer output_buffer;
  WhiteNoiseGen noise_gen;
};

TEST_F(PaulStretchTest, FxOff) {
  process_input(time_stretch);
  expect_all_near(input_buffer.get_read_ptr(), output_buffer.get_read_ptr(),
                  output_buffer.size());
}

TEST_F(PaulStretchTest, OutputStable) {
  // Recording
  process_input(time_stretch);
  // Processing
  time_stretch.set_recording(false);
  time_stretch.set_seek_position(0.0f);
  process_input(time_stretch);
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(output_buffer[i], 1e8) << "i = " << i;
  }
}

TEST_F(PaulStretchTest, NonZeroOut) {
  // Recording
  process_input(time_stretch);
  // Processing
  time_stretch.set_recording(false);
  process_input(time_stretch);
  auto* y = output_buffer.get_read_ptr();
  EXPECT_GT(compute_max(y, output_buffer.size()), 1e-3);
}

TEST_F(PaulStretchTest, ChangeFFTSize) {
  // Recording
  process_input(time_stretch);
  // First FFT size
  time_stretch.set_recording(false);
  time_stretch.set_seek_position(0.0f);
  process_input(time_stretch);
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(output_buffer[i], 1e8) << "i = " << i;
  }
  // Second FFT size
  time_stretch.set_fft_index(1);
  time_stretch.set_seek_position(0.0f);
  process_input(time_stretch);
  for (uint32_t i = 0; i < output_buffer.size(); i++) {
    EXPECT_LT(output_buffer[i], 1e8) << "i = " << i;
  }
}