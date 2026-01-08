#include <gtest/gtest.h>

#include "test_utilities.hpp"

using namespace audio_toolbox;

const uint32_t BUFF_LEN = 256;
const uint32_t BLOCK_LEN = 16;

class SeekableAudioBufferTest : public ::testing::Test {
 protected:
  void SetUp() override {
    white_noise.process(mono_buff.get_write_ptr(), BUFF_LEN, 1.0f);
  }

  void TearDown() override {}

  WhiteNoiseGen white_noise;
  unsigned int sample_rate = 16000;
  SeekableAudioBuffer<BUFF_LEN, BLOCK_LEN> mono_buff;
  std::array<real_type, BLOCK_LEN> block_buff;
};

TEST_F(SeekableAudioBufferTest, Constructor) {
  EXPECT_EQ(mono_buff.get_len(), BUFF_LEN);
  EXPECT_EQ(mono_buff.get_block_len(), BLOCK_LEN);
  EXPECT_EQ(mono_buff.get_samples_left(), BUFF_LEN);
  EXPECT_EQ(mono_buff.get_channels(), 1);
}

TEST_F(SeekableAudioBufferTest, Update) {
  auto* ptr_0 = mono_buff.get_seek_ptr();
  mono_buff.update();
  auto* ptr_1 = mono_buff.get_seek_ptr();
  EXPECT_EQ(ptr_0 + BLOCK_LEN, ptr_1);
  EXPECT_EQ(mono_buff.get_samples_left(), BUFF_LEN - BLOCK_LEN);
}

TEST_F(SeekableAudioBufferTest, Reset) {
  auto* ptr_0 = mono_buff.get_seek_ptr();
  mono_buff.update();
  mono_buff.reset();
  auto* ptr_1 = mono_buff.get_seek_ptr();
  EXPECT_EQ(ptr_0, ptr_1);
  EXPECT_EQ(mono_buff.get_samples_left(), BUFF_LEN);
}

TEST_F(SeekableAudioBufferTest, CopyBlockTo) {
  mono_buff.update();
  mono_buff.copy_block_to(block_buff.data());
  expect_all_near(block_buff.data(), mono_buff.get_seek_ptr(), BLOCK_LEN, 1e-6);
}