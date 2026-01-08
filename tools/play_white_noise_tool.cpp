/*
  Play white noise using audio_toolbox.
*/

#include <iostream>
#include <thread>

#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/math/random.hpp"

using namespace audio_toolbox;

constexpr unsigned int SAMPLE_RATE = 16000;
constexpr unsigned int IN_CHANNELS = 1;
constexpr unsigned int OUT_CHANNELS = 2;
constexpr unsigned int BLOCK_LEN = 256;
WhiteNoiseGen white_noise;

void process([[maybe_unused]] const real_type* input, real_type* output,
             unsigned int n_samples) {
  white_noise.process(output, n_samples * OUT_CHANNELS, 0.01);
}

int main() {
  AudioInterface audio_interface(SAMPLE_RATE, IN_CHANNELS, OUT_CHANNELS,
                                 BLOCK_LEN, AudioInterfaceDataType::float32,
                                 AudioInterfaceMode::Playback, process);

  // Playing white noise
  audio_interface.start();

  // Waiting for the user to stop the noise
  std::cout << "Press enter to stop the noise..." << std::endl;
  char c;
  std::cin.get(c);
  return 0;
}