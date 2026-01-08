/*
  Applies a specified filter to the input audio file and writes the result to
  the output file.
*/

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>

#include "audio_toolbox/core/audio_buffer.hpp"
#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/processors/virtual_analog_filters.hpp"

using namespace audio_toolbox;

constexpr uint32_t N_FILTERS = 6;
constexpr uint32_t N_CHANNELS = 1;
constexpr uint32_t SAMPLE_RATE = 16000;
constexpr float MAX_DURATION_S = 60 * 5;  // 5 minutes
constexpr uint32_t MAX_LEN = SAMPLE_RATE * static_cast<int>(MAX_DURATION_S);
AudioBuffer<MAX_LEN, N_CHANNELS> input_buff;
AudioBuffer<MAX_LEN, N_CHANNELS> output_buff;

/**
 * @brief Prints the available filter types.
 */
void print_filter_types(auto filter_types) {
  std::cout << "Available filter types: ";
  std::for_each(filter_types.begin(), filter_types.end(),
                [](const std::string& s) { std::cout << s << " "; });
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  real_type cutoff_hz;
  real_type resonance;
  std::array<std::string, N_FILTERS> filter_types = {"lowpass1p", "highpass1p",
                                                     "lowpass2p", "highpass2p",
                                                     "lowpass4p", "highpass4p"};

  // Parse arguments
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> <output_file> <filter_type> <cutoff_hz> "
                 "<resonance_[0, 1]>"
              << std::endl;
    print_filter_types(filter_types);
    return 1;
  }
  std::string input_file{argv[1]};
  std::string output_file{argv[2]};
  std::string selected_filter{argv[3]};
  cutoff_hz = clip(std::stof(argv[4]), 0.0f, SAMPLE_RATE / 2.0f);
  resonance = clip(std::stof(argv[5]), 0.0f, 1.0f);

  // Error handling
  if (!contains(filter_types, selected_filter)) {
    std::cerr << "Invalid filter type." << std::endl;
    print_filter_types(filter_types);
    return 1;
  }

  // Processing
  std::array<std::unique_ptr<AudioFilterBase<SAMPLE_RATE>>, N_FILTERS> filters =
      {std::make_unique<VALowpass1P<SAMPLE_RATE>>(),
       std::make_unique<VAHighpass1P<SAMPLE_RATE>>(),
       std::make_unique<VALowpassLadder2P<SAMPLE_RATE>>(),
       std::make_unique<VAHighpassLadder2P<SAMPLE_RATE>>(),
       std::make_unique<VALowpassLadder4P<SAMPLE_RATE>>(),
       std::make_unique<VAHighpassLadder4P<SAMPLE_RATE>>()};
  auto idx = index_of(filter_types, selected_filter);
  auto& filter = filters[idx];
  filter->set_freq(cutoff_hz);
  filter->set_resonance(resonance);

  std::cout << "Selected filter: " << selected_filter << std::endl;
  std::cout << "Cutoff frequency: " << cutoff_hz << " Hz" << std::endl;
  std::cout << "Resonance: " << resonance << std::endl;
  std::cout << "Processing started" << std::endl;
  auto in_len = read_audio_file(input_file.c_str(), input_buff.get_write_ptr(),
                                MAX_LEN, N_CHANNELS, SAMPLE_RATE);
  filter->process(input_buff.get_read_ptr(), output_buff.get_write_ptr(),
                  in_len);
  write_audio_file(output_file.c_str(), output_buff.get_read_ptr(), in_len,
                   N_CHANNELS, SAMPLE_RATE);
  std::cout << output_file << " saved" << std::endl;

  return 0;
}