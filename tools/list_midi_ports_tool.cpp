/*
  List all available MIDI input and output ports.
*/

#include <rtmidi/RtMidi.h>

#include <iostream>

int main() {
  RtMidiIn midi_in;
  RtMidiOut midi_out;

  unsigned int n_ports_in = midi_in.getPortCount();
  unsigned int n_ports_out = midi_out.getPortCount();

  std::cout << "MIDI input ports: " << n_ports_in << std::endl;
  for (unsigned int i = 0; i < n_ports_in; i++) {
    std::cout << "  " << i << ": " << midi_in.getPortName(i) << std::endl;
  }

  std::cout << "MIDI output ports: " << n_ports_out << std::endl;
  for (unsigned int i = 0; i < n_ports_out; i++) {
    std::cout << "  " << i << ": " << midi_out.getPortName(i) << std::endl;
  }

  return 0;
}