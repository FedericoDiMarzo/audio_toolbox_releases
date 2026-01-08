/*
  Filter white noise using a virtual analog filter.
*/

#include <array>
#include <cstdint>
#include <iostream>
#include <thread>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/core/audio_parameter.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/virtual_analog_filters.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 16000;
constexpr uint32_t IN_CHANNELS = 1;
constexpr uint32_t OUT_CHANNELS = 2;
constexpr uint32_t BLOCK_LEN = 256;
constexpr real_type PARAM_TRANSITION_MS = 500.0f;

WhiteNoiseGen white_noise;
AudioParameter cutoff(1000.0f, SAMPLE_RATE, BLOCK_LEN, PARAM_TRANSITION_MS);
VALowpassLadder4P<SAMPLE_RATE> lpf(cutoff.get(), 1.0f);
VAHighpassLadder4P<SAMPLE_RATE> hpf(cutoff.get(), 1.0f);

// Select the filter to use
auto& filter = hpf;

std::array<AudioBuffer<BLOCK_LEN>, 2> buffers_mono;
std::array<AudioBuffer<BLOCK_LEN, 2>, 1> buffers_stereo;

void process([[maybe_unused]] const real_type* input, real_type* output,
             uint32_t n_samples) {
  cutoff.update();
  filter.set_freq(cutoff.get());
  white_noise.process(buffers_mono[0].get_write_ptr(), n_samples, 0.2);
  filter.process(buffers_mono[0].get_read_ptr(),
                 buffers_mono[1].get_write_ptr(), n_samples);
  // HPF
  buffers_stereo[0].interleave_from(buffers_mono[1], buffers_mono[1]);
  buffers_stereo[0].copy_to(output);
}

void handle_inputs() {
  real_type fc = cutoff.get();
  std::cout << "Enter the cutoff frequency (Hz) and press enter to change it"
            << std::endl
            << "Press ctrl+c to exit" << std::endl;

  while (true) {
    std::cin >> fc;
    cutoff.set(fc);
    std::cout << "Cutoff frequency: " << fc << " Hz" << std::endl;
  }
}

int main() {
  AudioInterface audio_interface(SAMPLE_RATE, IN_CHANNELS, OUT_CHANNELS,
                                 BLOCK_LEN, AudioInterfaceDataType::float32,
                                 AudioInterfaceMode::Playback, process);

  // Playing white noise
  audio_interface.start();

  // User interface in the main thread
  handle_inputs();

  return 0;
}