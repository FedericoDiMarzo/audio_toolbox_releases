/*
  Apply N diffusion blocks to an input.
*/

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

#include "audio_toolbox/core/audio_interface.hpp"
#include "audio_toolbox/processors/reverbs.hpp"

using namespace audio_toolbox;

constexpr uint32_t SAMPLE_RATE = 48000;
constexpr uint32_t OUT_CHANNELS = 2;
constexpr uint32_t DIFF_CHANNELS = 8;
constexpr uint32_t MAX_STEPS = 64;
constexpr uint32_t MAX_DLY = 1 << 8;
constexpr uint32_t FB_MAX_DELAY = 1 << 7;
constexpr uint32_t WAV_IN_MAX_LEN = SAMPLE_RATE * 60 * 2;

using Reverb = ReverbDiffuse<SAMPLE_RATE, 1, OUT_CHANNELS, DIFF_CHANNELS,
                             MAX_DLY, FB_MAX_DELAY>;
Reverb reverb;

AudioBuffer<WAV_IN_MAX_LEN> in_buffer;
AudioBuffer<WAV_IN_MAX_LEN, OUT_CHANNELS> out_buffer;
std::array<AudioBuffer<WAV_IN_MAX_LEN, DIFF_CHANNELS>, 4> diff_buffers;

int main(int argc, char** argv) {
  // Parse arguments and load audio file
  if (argc != 9) {
    std::cout << "Usage: " << argv[0] << " <in_wav>"
              << " <out_wav>"
              << " <steps>"
              << " <lfo>"
              << " <alpha_residual>"
              << " <feedback>"
              << " <decay[0,1]>"
              << " <mix[0,1]>" << std::endl;

    exit(1);
  }
  std::string in_wav = argv[1];
  std::string out_wav = argv[2];
  uint32_t num_steps = atoi(argv[3]);
  num_steps = std::min(num_steps, MAX_STEPS);
  real_type lfo_to_diffusion = atof(argv[4]);
  real_type alpha_residual = atof(argv[5]);
  real_type feedback = atof(argv[6]);
  real_type decay = clip(atof(argv[7]), 0.0f, 1.0f);
  real_type mix = clip(atof(argv[8]), 0.0f, 1.0f);

  // Pointers
  auto* x = in_buffer.get_write_ptr();
  auto* y = out_buffer.get_write_ptr();

  // Reverb setup
  reverb.set_alpha_residual(alpha_residual);
  reverb.set_feedback_gain(feedback);
  reverb.set_diffusion_steps(num_steps);
  reverb.set_lfo_to_diffusion(lfo_to_diffusion);
  reverb.set_decay(decay);

  // Reading input
  uint32_t n_samples =
      read_audio_file(in_wav, x, WAV_IN_MAX_LEN, 1, SAMPLE_RATE);

  // Processing
  std::cout << "Processing " << in_wav << " with " << std::endl
            << num_steps << " steps " << std::endl
            << lfo_to_diffusion << " lfo_to_diffusion" << std::endl
            << alpha_residual << " alpha_residual" << std::endl
            << feedback << " feedback" << std::endl
            << decay << " decay" << std::endl
            << mix << " mix" << std::endl;

  reverb.process(x, y, n_samples);
  for (uint32_t i = 0; i < n_samples; ++i) {
    for (uint32_t j = 0; j < OUT_CHANNELS; ++j) {
      y[i * OUT_CHANNELS + j] =
          mix * y[i * OUT_CHANNELS + j] + (1.0f - mix) * x[i];
    }
  }

  // Writing output
  write_audio_file(out_wav, y, n_samples, OUT_CHANNELS, SAMPLE_RATE);
  std::cout << "Saved " << out_wav << std::endl;
}