/*
  Testing PaulStretch algorithm.
*/

#include <iostream>
#include <thread>

#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/core/audio_parameter.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/paulstretch.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 48000;
constexpr uint32_t IN_CHANNELS = 1;
constexpr uint32_t OUT_CHANNELS = 2;
constexpr uint32_t WIN_LEN_A = 8192;
constexpr std::array<uint32_t, 1> FFT_LEN_ARRAY = {WIN_LEN_A};

constexpr uint32_t BLOCK_LEN = 256;
constexpr uint32_t REC_BUFFER_LEN = next_power_of_two(SAMPLE_RATE * 3);
constexpr uint32_t WAV_BUFFER_LEN = SAMPLE_RATE * 60 * 5;
constexpr real_type TRANSITION_SMOOTHING_MS = 3000.0f;

AudioBuffer<BLOCK_LEN> mono_buff;
AudioBuffer<BLOCK_LEN, 2> stereo_buff;
SeekableAudioBuffer<WAV_BUFFER_LEN, BLOCK_LEN> seekable_buff;

// For smooth transitions
AudioParameter seek_smoothed(1.0f, SAMPLE_RATE, BLOCK_LEN,
                             TRANSITION_SMOOTHING_MS);

using TimeStretch = PaulStretch<SAMPLE_RATE, 1, FFT_LEN_ARRAY, REC_BUFFER_LEN>;

TimeStretch time_stretch;

void process([[maybe_unused]] const real_type* _, real_type* output,
             [[maybe_unused]] uint32_t n_samples) {
  // Update seek position
  time_stretch.set_seek_position(seek_smoothed.get());
  seek_smoothed.update();

  // Read from wav
  auto* x = seekable_buff.get_seek_ptr();
  seekable_buff.update();

  // Process
  time_stretch.process(x, mono_buff.get_write_ptr(), BLOCK_LEN);
  stereo_buff.interleave_from(mono_buff, mono_buff);
  stereo_buff.copy_to(output);
}

int main(int argc, char** argv) {
  // Parse arguments
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <wav_file>" << std::endl;
    exit(1);
  }
  std::string wav_file = argv[1];

  // Read wav file
  auto loop_len = read_audio_file(wav_file, seekable_buff.get_write_ptr(),
                                  WAV_BUFFER_LEN, IN_CHANNELS, SAMPLE_RATE);
  seekable_buff.set_loop_len(loop_len);

  // Run audio interface
  AudioInterface audio_interface(SAMPLE_RATE, IN_CHANNELS, OUT_CHANNELS,
                                 BLOCK_LEN, AudioInterfaceDataType::float32,
                                 AudioInterfaceMode::Playback, process);
  audio_interface.start();

  // User input
  char c = '\0';
  std::cout << "Press 'q' to quit" << std::endl;
  std::cout << "Press 'r' to switch to recording mode" << std::endl;
  std::cout << "Press 'p' to switch to processing mode" << std::endl;
  std::cout << "Press a number between 0 and 9 to seek in the buffer"
            << std::endl;
  while (c != 'q') {
    std::cin.get(c);
    switch (c) {
      case 'r':
        time_stretch.set_recording(true);
        std::cout << "Recording mode on" << std::endl;
        break;
      case 'p':
        time_stretch.set_recording(false);
        std::cout << "Processing mode on" << std::endl;
        break;
      default:
        if (c >= '0' && c <= '9') seek_smoothed.set((c - '0') / 9.0);
        break;
    }
  }

  return 0;
}