/*
  Prints all the received CC messages from all MIDI input ports.
*/

#include <rtmidi/RtMidi.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "audio_toolbox/midi/midi.hpp"

using namespace audio_toolbox;

constexpr uint32_t MIDI_READ_INTERVAL_MS = 5;

int main(int argc, char** argv) {
  // Parse arguments
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <midi_in_index>" << std::endl;
    exit(1);
  }
  unsigned int midi_in_index = std::stoi(argv[1]);

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

  // Print all received MIDI CC messages
  std::cout << "Listening to MIDI CC messages..." << std::endl;
  std::cout << "Press Ctrl+C to Quit" << std::endl;
  while (true) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MIDI_READ_INTERVAL_MS));
    midi_tokens.clear();
    midi_in.getMessage(&midi_bytes);
    midi_parser.parse(midi_bytes, midi_tokens);
    for (const auto& token : midi_tokens) {
      // Print CC messages only
      if (midi_event_is_type(token, MidiStatusType::ControlChange)) {
        const auto cc = token.as_control_change();
        std::cout << "CC" << static_cast<int>(cc.control_number) << ": "
                  << static_cast<int>(cc.value) << std::endl;
      }
    }
  }
  return 0;
}