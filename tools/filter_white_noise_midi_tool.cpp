/*
  Filter white noise using a virtual analog filter.
  The cutoff frequency and resonance can be changed in real-time using
  MIDI CC messages.
*/

#include <rtmidi/RtMidi.h>

#include <array>
#include <iostream>
#include <thread>
#include <vector>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/core/audio_parameter.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/midi/midi.hpp"
#include "audio_toolbox/processors/virtual_analog_filters.hpp"

using namespace audio_toolbox;

constexpr uint32_t MIDI_READ_INTERVAL_MS = 5;
constexpr uint32_t MIDI_CC_CUTOFF = 23;
constexpr uint32_t MIDI_CC_RESONANCE = 83;
constexpr uint32_t SAMPLE_RATE = 16000;
constexpr uint32_t IN_CHANNELS = 1;
constexpr uint32_t OUT_CHANNELS = 2;
constexpr uint32_t BLOCK_LEN = 128;
constexpr real_type PARAM_TRANSITION_MS = 1;

AudioBuffer<BLOCK_LEN> buffer_0;
AudioBuffer<BLOCK_LEN> buffer_1;
AudioBuffer<BLOCK_LEN, 2> stereo_buffer;
WhiteNoiseGen white_noise;
AudioParameter cutoff(40.0f, SAMPLE_RATE, 1, PARAM_TRANSITION_MS);
AudioParameter resonance(0.0f, SAMPLE_RATE, 1, PARAM_TRANSITION_MS);
AudioParameterDispatcher audio_parameter_dispatcher;
MidiCCParameter cutoff_cc(MIDI_CC_CUTOFF, 40, SAMPLE_RATE / 2);
MidiCCParameter resonance_cc(MIDI_CC_RESONANCE, 0, 1);
MidiCCDispatcher midi_cc_dispatcher;
VALowpassLadder4P<SAMPLE_RATE> filter(cutoff.get(), resonance.get());

void process([[maybe_unused]] const real_type* input, real_type* output,
             uint32_t n_samples) {
  white_noise.process(buffer_0.get_write_ptr(), n_samples, 0.01);
  auto x = buffer_0.get_read_ptr();
  auto y = buffer_1.get_write_ptr();
  for (uint32_t i = 0; i < n_samples; ++i) {
    audio_parameter_dispatcher.update();
    filter.set_freq(cutoff.get());
    filter.set_resonance(resonance.get());
    filter.process(x, y, 1);
    x++;
    y++;
  }
  stereo_buffer.interleave_from(buffer_1, buffer_1);
  stereo_buffer.copy_to(output);
}

void handle_midi(RtMidiIn& midi_reader, MidiParser& midi_parser,
                 std::vector<uint8_t>& midi_bytes,
                 std::vector<MidiToken>& midi_tokens) {
  while (true) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MIDI_READ_INTERVAL_MS));
    // Midi reading and parsing
    midi_reader.getMessage(&midi_bytes);
    if (midi_parser.parse(midi_bytes, midi_tokens)) {
      midi_cc_dispatcher.update(midi_tokens);
      midi_tokens.clear();
    }
    // Apply changes to parameters
    cutoff.set(cutoff_cc.get());
    resonance.set(resonance_cc.get());
  }
}

int main(int argc, char** argv) {
  // Parse arguments
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <midi_in_index>" << std::endl;
    exit(1);
  }
  uint32_t midi_in_index = std::stoi(argv[1]);

  // Initialize audio interface
  AudioInterface audio_interface(SAMPLE_RATE, IN_CHANNELS, OUT_CHANNELS,
                                 BLOCK_LEN, AudioInterfaceDataType::float32,
                                 AudioInterfaceMode::Playback, process);

  // Open MIDI input port
  RtMidiIn midi_in;
  MidiParser midi_parser;
  std::vector<uint8_t> midi_bytes;
  std::vector<MidiToken> midi_tokens;
  if (midi_in_index >= midi_in.getPortCount()) {
    std::cout << "Invalid MIDI input port index." << std::endl;
    exit(1);
  }
  midi_in.openPort(midi_in_index);
  std::cout << midi_in.getPortName(midi_in_index) << " selected" << std::endl;

  // Setting dispatchers
  audio_parameter_dispatcher.add_observer(cutoff);
  audio_parameter_dispatcher.add_observer(resonance);
  midi_cc_dispatcher.add_observer(cutoff_cc);
  midi_cc_dispatcher.add_observer(resonance_cc);

  // Playing white noise
  audio_interface.start();

  // MIDI CC messages handled in the main thread
  handle_midi(midi_in, midi_parser, midi_bytes, midi_tokens);

  return 0;
}