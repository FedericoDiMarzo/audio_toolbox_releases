// #include "audio_toolbox/processors/delays.hpp"

// #include "audio_toolbox/processors/oscillators.hpp"
// #include "audio_toolbox/static/audio_buffer_static.hpp"
// #include "test_utilities.hpp"

// using namespace audio_toolbox;

// class DelaysTest : public ::testing::Test
// {
//   protected:
//     void SetUp() override
//     {
//         set_random_seed(1);
//         auto* x = input_buffer.get_write_ptr();
//         auto* y0 = output_buffers[0].get_write_ptr();
//         auto* y1 = output_buffers[1].get_write_ptr();
//         auto* y2 = output_buffers[2].get_write_ptr();
//         noise_gen.process(x, input_buffer.size());
//         delay_int.process(x, y0, input_buffer.size());
//         delay_frac.process(x, y1, input_buffer.size());
//         mod_delay.process(x, y2, input_buffer.size());
//     }

//     void TearDown() override {}

//     static constexpr uint32_t MAX_DLY = 8;
//     static constexpr uint32_t INT_DLY = 2;
//     static constexpr real_type FRAC_DLY = 2.999f;
//     static constexpr uint32_t BUFFER_SIZE = 16;
//     static constexpr uint32_t SAMPLE_RATE = 16000;

//     using Buffer = AudioBufferStatic<BUFFER_SIZE>;
//     using LFO = LookupOscillator<SAMPLE_RATE, 128>;

//     IntegerDelay<MAX_DLY> delay_int{ INT_DLY };
//     FractionalDelay<MAX_DLY> delay_frac{ FRAC_DLY };
//     ModulatedFractionalDelay<MAX_DLY, LFO> mod_delay{ FRAC_DLY };
//     Buffer input_buffer;
//     std::array<Buffer, 3> output_buffers;
//     WhiteNoiseGen noise_gen;
// };

// // IntegerDelay - - - - - - - - - - - - - -

// TEST_F(DelaysTest, IntegerDelayGetDelay)
// {
//     EXPECT_EQ(delay_int.get_delay(), INT_DLY);
// }

// TEST_F(DelaysTest, IntegerDelayReset)
// {
//     auto* x = input_buffer.get_read_ptr();
//     delay_int.reset();
//     for (uint32_t i = 0; i < INT_DLY; i++)
//     {
//         real_type y;
//         delay_int.process_sample(x + i, &y);
//         EXPECT_NEAR(0.0f, y, 1e-6) << "i = " << i;
//     }
// }

// TEST_F(DelaysTest, IntegerDelayRandomize)
// {
//     uint32_t rand_min = 1;
//     uint32_t rand_max = 6;
//     for (uint32_t i = 0; i < 100; i++)
//     {
//         delay_int.set_random_range(rand_min, rand_max);
//         EXPECT_GE(delay_int.get_delay(), rand_min);
//         EXPECT_LE(delay_int.get_delay(), rand_max);
//     }
// }

// // FractionalDelay - - - - - - - - - - - - - -

// TEST_F(DelaysTest, FractionalDelayGetDelay)
// {
//     EXPECT_NEAR(delay_frac.get_delay(), FRAC_DLY, 1e-6);
// }

// TEST_F(DelaysTest, FractionalDelayRandomize)
// {
//     real_type rand_min = 1.1;
//     real_type rand_max = 6.6;
//     for (uint32_t i = 0; i < 100; i++)
//     {
//         delay_int.set_random_range(rand_min, rand_max);
//         EXPECT_GE(delay_int.get_delay(), rand_min);
//         EXPECT_LE(delay_int.get_delay(), rand_max);
//     }
// }

// // ModulatedFractionalDelay - - - - - - - - - - - - - -

// TEST_F(DelaysTest, ModulatedFractionalDelayGetDelay)
// {
//     EXPECT_NEAR(delay_frac.get_delay(), FRAC_DLY, 1e-6);
// }

// TEST_F(DelaysTest, ModulatedFractionalDelayProcess)
// {
//     real_type frac = FRAC_DLY - std::floor(FRAC_DLY);
//     auto* x = input_buffer.get_read_ptr();
//     auto* y = output_buffers[1].get_read_ptr();
//     EXPECT_NEAR(y[0], 0.0f, 1e-6);
//     EXPECT_NEAR(y[1], 0.0f, 1e-6);
//     EXPECT_NEAR(y[2], linear_interpolation(x[0], 0.0f, frac), 1e-6);
//     for (uint32_t i = 3; i < BUFFER_SIZE; i++)
//     {
//         real_type interp = linear_interpolation(x[i - 2], x[i - 3], frac);
//         EXPECT_NEAR(interp, y[i], 1e-6) << "i = " << i;
//     }
// }

// TEST_F(DelaysTest, ModulatedFractionalDelayRandomize)
// {
//     real_type rand_min = 1.1;
//     real_type rand_max = 6.6;
//     for (uint32_t i = 0; i < 100; i++)
//     {
//         delay_int.set_random_range(rand_min, rand_max);
//         EXPECT_GE(delay_int.get_delay(), rand_min);
//         EXPECT_LE(delay_int.get_delay(), rand_max);
//     }
// }