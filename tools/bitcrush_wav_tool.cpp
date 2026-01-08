/*
  Testing BitCrush algorithm.
*/

#include <iostream>
#include <string>
#include <thread>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/processors/bitcrush.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 48000;
constexpr uint32_t IN_CHANNELS = 1;
constexpr uint32_t OUT_CHANNELS = 1;
constexpr uint32_t BLOCK_LEN = 512;
constexpr float MAX_DURATION_S = 10;
constexpr uint32_t MAX_LEN = SAMPLE_RATE * static_cast<int>(MAX_DURATION_S);
SeekableAudioBuffer<MAX_LEN, BLOCK_LEN> buffer_wav;
std::unique_ptr<BitCrush> bitcrush;

void process([[maybe_unused]] const real_type* input, real_type* output,
             [[maybe_unused]] uint32_t n_samples) {
  // BitCrushing
  bitcrush->process(buffer_wav.get_seek_ptr(), output, n_samples);

  // Seeking forward
  buffer_wav.update();
}

int main(int argc, char** argv) {
  // Parse arguments and load audio file
  if (argc < 5) {
    std::cout << "Usage: " << argv[0] << " <wav_file>"
              << " <bit_depth>"
              << " <downsample_factor>"
              << " <clip_threshold>" << std::endl;

    exit(1);
  }
  std::string wav_file = argv[1];
  int bit_depth = atoi(argv[2]);
  int downsample_factor = atoi(argv[3]);
  float clip_threshold = (float)atof(argv[4]);
  read_audio_file(wav_file, buffer_wav.get_write_ptr(), MAX_LEN, IN_CHANNELS,
                  SAMPLE_RATE);

  // Creating the BitCrush object
  bitcrush =
      std::make_unique<BitCrush>(bit_depth, downsample_factor, clip_threshold);

  AudioInterface audio_interface(SAMPLE_RATE, IN_CHANNELS, OUT_CHANNELS,
                                 BLOCK_LEN, AudioInterfaceDataType::float32,
                                 AudioInterfaceMode::Playback, process);

  // Playing wav file
  audio_interface.start();

  // Waiting for the user to stop the playback
  std::cout << "Press enter to stop the playback." << std::endl;
  char c = 0;
  while (c != '\n') {
    std::cin.get(c);
  }
  return 0;
}