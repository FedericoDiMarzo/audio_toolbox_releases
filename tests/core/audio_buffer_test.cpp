#include "audio_toolbox/core/audio_buffer.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

const uint32_t BUFF_LEN = 256;

class AudioBufferTest : public ::testing::Test {
 protected:
  void SetUp() override {
    white_noise.process(mono_real_buff.get_write_ptr(), BUFF_LEN, 1.0f);
    white_noise.process(stereo_real_buff.get_write_ptr(), 2 * BUFF_LEN, 1.0f);
  }

  void TearDown() override {}

  WhiteNoiseGen white_noise;
  AudioBuffer<BUFF_LEN, 1, real_type> mono_real_buff;
  AudioBuffer<BUFF_LEN, 2, real_type> stereo_real_buff;
  AudioBuffer<BUFF_LEN, 1, std::complex<real_type>> mono_cpx_buff;
  AudioBuffer<BUFF_LEN, 2, std::complex<real_type>> stereo_cpx_buff;
};

TEST_F(AudioBufferTest, ConstructorMono) {
  EXPECT_EQ(mono_real_buff.get_len(), BUFF_LEN);
  EXPECT_EQ(mono_real_buff.get_channels(), 1);
}

TEST_F(AudioBufferTest, ConstructorStereo) {
  EXPECT_EQ(stereo_real_buff.get_len(), BUFF_LEN);
  EXPECT_EQ(stereo_real_buff.get_channels(), 2);
}

TEST_F(AudioBufferTest, ClearMonoReal) {
  mono_real_buff.clear();
  auto zeros = get_zeros<real_type, BUFF_LEN>();
  expect_all_near(zeros.data(), mono_real_buff.get_read_ptr(), BUFF_LEN, 1e-6);
}

TEST_F(AudioBufferTest, ClearStereoReal) {
  stereo_real_buff.clear();
  auto zeros = get_zeros<real_type, 2 * BUFF_LEN>();
  expect_all_near(zeros.data(), stereo_real_buff.get_read_ptr(), 2 * BUFF_LEN,
                  1e-6);
}

TEST_F(AudioBufferTest, ClearMonoCpx) {
  mono_cpx_buff.clear();
  auto zeros = get_zeros<std::complex<real_type>, BUFF_LEN>();
  expect_all_near_cpx(zeros.data(), mono_cpx_buff.get_read_ptr(), BUFF_LEN,
                      1e-6);
}

TEST_F(AudioBufferTest, ClearStereoCpx) {
  stereo_cpx_buff.clear();
  auto zeros = get_zeros<std::complex<real_type>, 2 * BUFF_LEN>();
  expect_all_near_cpx(zeros.data(), stereo_cpx_buff.get_read_ptr(),
                      2 * BUFF_LEN, 1e-6);
}

TEST_F(AudioBufferTest, CopyTo) {
  std::array<real_type, BUFF_LEN> copy;
  mono_real_buff.copy_to(copy.data());
  expect_all_near(copy.data(), mono_real_buff.get_read_ptr(), BUFF_LEN, 1e-6);
}

TEST_F(AudioBufferTest, CopyFromChannel) {
  unsigned int channel = 0;
  AudioBuffer<BUFF_LEN, 1, real_type> channel_copy;
  stereo_real_buff.copy_from_channel(channel_copy, channel);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    EXPECT_EQ(channel_copy.get_read_ptr()[i],
              stereo_real_buff.get_read_ptr()[2 * i + channel]);
  }
}

TEST_F(AudioBufferTest, CopyToChannel) {
  unsigned int channel = 0;
  stereo_real_buff.copy_to_channel(mono_real_buff, channel);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    EXPECT_EQ(mono_real_buff.get_read_ptr()[i],
              stereo_real_buff.get_read_ptr()[2 * i + channel]);
  }
}

TEST_F(AudioBufferTest, InterleaveFrom) {
  stereo_real_buff.interleave_from(mono_real_buff, mono_real_buff);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    EXPECT_EQ(mono_real_buff.get_read_ptr()[i],
              stereo_real_buff.get_read_ptr()[2 * i]);
    EXPECT_EQ(mono_real_buff.get_read_ptr()[i],
              stereo_real_buff.get_read_ptr()[2 * i + 1]);
  }
}

TEST_F(AudioBufferTest, Scale) {
  float scaling = 2.0f;
  std::array<real_type, BUFF_LEN> copy;
  mono_real_buff.copy_to(copy.data());
  mono_real_buff.scale(scaling);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    copy[i] *= scaling;
  }
  expect_all_near(copy.data(), mono_real_buff.get_read_ptr(), BUFF_LEN, 1e-6);
}

TEST_F(AudioBufferTest, ScaleDB) {
  float scaling_db = 6.0f;
  float scaling_linear = compute_inverse_db(scaling_db);
  std::array<real_type, BUFF_LEN> copy;
  mono_real_buff.copy_to(copy.data());
  mono_real_buff.scale_db(scaling_db);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    copy[i] *= scaling_linear;
  }
  expect_all_near(copy.data(), mono_real_buff.get_read_ptr(), BUFF_LEN, 1e-6);
}

TEST_F(AudioBufferTest, OperatorSquareBrackets) {
  auto ones = get_ones<real_type, BUFF_LEN>();
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    mono_real_buff[i] = 1.0;
  }
  expect_all_near(ones.data(), mono_real_buff.get_read_ptr(), BUFF_LEN, 1e-6);
}

TEST_F(AudioBufferTest, ToComplex) {
  AudioBuffer<2 * BUFF_LEN, 1, real_type> interleaved_buff;
  white_noise.process(interleaved_buff.get_write_ptr(), 2 * BUFF_LEN, 1.0f);
  audio_buffer_to_complex(interleaved_buff, mono_cpx_buff);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    EXPECT_EQ(mono_cpx_buff.get_read_ptr()[i].real(),
              interleaved_buff.get_read_ptr()[2 * i]);
    EXPECT_EQ(mono_cpx_buff.get_read_ptr()[i].imag(),
              interleaved_buff.get_read_ptr()[2 * i + 1]);
  }
}
TEST_F(AudioBufferTest, FromComplex) {
  AudioBuffer<2 * BUFF_LEN, 1, real_type> interleaved_buff;
  audio_buffer_from_complex(interleaved_buff, mono_cpx_buff);
  for (unsigned int i = 0; i < BUFF_LEN; i++) {
    EXPECT_EQ(mono_cpx_buff.get_read_ptr()[i].real(),
              interleaved_buff.get_read_ptr()[2 * i]);
    EXPECT_EQ(mono_cpx_buff.get_read_ptr()[i].imag(),
              interleaved_buff.get_read_ptr()[2 * i + 1]);
  }
}
