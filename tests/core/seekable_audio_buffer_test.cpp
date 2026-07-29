#include "audio_toolbox/core/seekable_audio_buffer.hpp"

#include <gtest/gtest.h>

#include "test_utilities.hpp"

using namespace audio_toolbox;

const uint32_t BUFF_LEN = 256;
const uint32_t UPDATE_LEN = 16;

class SeekableAudioBufferTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        white_noise.process(mono_buff.get_write_ptr(), BUFF_LEN, 1.0f);
    }

    void TearDown() override {}

    WhiteNoiseGen white_noise;
    unsigned int sample_rate = 16000;
    SeekableAudioBuffer mono_buff{ BUFF_LEN, 1, UPDATE_LEN };
    std::array<real_type, UPDATE_LEN> block_buff;
};

TEST_F(SeekableAudioBufferTest, Constructor)
{
    EXPECT_EQ(mono_buff.get_num_samples(), BUFF_LEN);
    EXPECT_EQ(mono_buff.get_update_block_len(), UPDATE_LEN);
    EXPECT_EQ(mono_buff.get_samples_left(), BUFF_LEN);
    EXPECT_EQ(mono_buff.get_num_channels(), 1);
}

TEST_F(SeekableAudioBufferTest, Update)
{
    auto* ptr_0 = mono_buff.get_seek_ptr();
    mono_buff.update();
    auto* ptr_1 = mono_buff.get_seek_ptr();
    EXPECT_EQ(ptr_0 + UPDATE_LEN, ptr_1);
    EXPECT_EQ(mono_buff.get_samples_left(), BUFF_LEN - UPDATE_LEN);
}

TEST_F(SeekableAudioBufferTest, Reset)
{
    auto* ptr_0 = mono_buff.get_seek_ptr();
    mono_buff.update();
    mono_buff.reset();
    auto* ptr_1 = mono_buff.get_seek_ptr();
    EXPECT_EQ(ptr_0, ptr_1);
    EXPECT_EQ(mono_buff.get_samples_left(), BUFF_LEN);
}

TEST_F(SeekableAudioBufferTest, CopyBlockTo)
{
    mono_buff.update();
    mono_buff.copy_block_to(block_buff.data());
    expect_all_near(block_buff.data(), mono_buff.get_seek_ptr(), UPDATE_LEN, 1e-6);
}