/*
  Load and play a wav file.
*/

#include <iostream>
#include <thread>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/audio_interface.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 48000;
constexpr uint32_t IN_CHANNELS = 1;
constexpr uint32_t OUT_CHANNELS = 2;
constexpr uint32_t BLOCK_LEN = 512;
constexpr real_type MAX_DURATION_S = 10;
constexpr uint32_t MAX_LEN = SAMPLE_RATE * static_cast<int>(MAX_DURATION_S);
SeekableAudioBuffer<MAX_LEN, BLOCK_LEN, OUT_CHANNELS> buffer_wav;

void process([[maybe_unused]] const real_type* input, real_type* output,
             [[maybe_unused]] uint32_t n_samples) {
  // Copying from buffer to output
  buffer_wav.copy_block_to(output);

  // Seeking forward
  buffer_wav.update();
}

int main(int argc, char** argv) {
  // Parse arguments and load audio file
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <wav_file>" << std::endl;
    exit(1);
  }
  std::string wav_file = argv[1];
  read_audio_file(wav_file, buffer_wav.get_write_ptr(), MAX_LEN, OUT_CHANNELS,
                  SAMPLE_RATE);

  AudioInterface audio_interface(SAMPLE_RATE, IN_CHANNELS, OUT_CHANNELS,
                                 BLOCK_LEN, AudioInterfaceDataType::float32,
                                 AudioInterfaceMode::Playback, process);

  // Playing wav file
  audio_interface.start();

  // Waiting for the user to stop the playback
  std::cout << "Press enter to stop the playback..." << std::endl;
  char c;
  std::cin.get(c);
  return 0;
}