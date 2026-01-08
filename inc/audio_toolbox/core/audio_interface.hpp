#ifndef _AUDIO_TOOLBOX_AUDIO_INTERFACE_H_
#define _AUDIO_TOOLBOX_AUDIO_INTERFACE_H_

/*
  Class that handles the audio interface using Miniaudio.
  Currently only ALSA on Linux is supported, but other backends
  may be enabled and tested in the future.
*/

#include <cstdint>
#include <functional>
#include <string>

#include "audio_toolbox/core/miniaudio_instance.hpp"
#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"

/**
 * @brief Callback function used to process the audio data.
 *
 * @param input Pointer to the input buffer.
 * @param output Pointer to the output buffer.
 * @param n_samples Number of samples to process.
 */
using AudioInterfaceCallback =
    std::function<void(const real_type*, real_type*, uint32_t)>;

namespace audio_toolbox {

/**
 * @brief Reads an audio file and stores it in a buffer.
 *
 * @param filename Path to the audio file.
 * @param buffer Buffer used to store the audio data.
 * @param max_len Maximum number of samples to read.
 * @param n_channels Number of channels to read.
 * @param sample_rate Target sample rate.
 * @return Number of samples read.
 */
uint32_t read_audio_file(std::string filename, real_type* buffer,
                         uint32_t max_len, uint32_t n_channels,
                         uint32_t sample_rate);

/**
 * @brief Writes a buffer to an audio file.
 *
 * @param filename Path to the audio file.
 * @param buffer Buffer containing the audio data.
 * @param n_samples Number of samples to write.
 * @param n_channels Number of channels to write.
 * @param sample_rate Sample rate of the audio data.
 */
void write_audio_file(std::string filename, const real_type* buffer,
                      uint32_t n_samples, uint32_t n_channels,
                      uint32_t sample_rate);

/**
 * @brief Specifies the data type used for the audio interface.
 *
 * @note Only float32 is supported for now.
 * @var float32 32-bit floating point between -1.0 and +1.0.
 */
enum class AudioInterfaceDataType {
  float32,
  // TODO: Handle other datatypes (or not?)
  // INT16,
  // INT32,
  // UINT8,
};

/**
 * @brief Specifies the mode of the audio interface.
 *
 * @var Playback Playback only.
 * @var Capture Capture only.
 * @var Duplex Playback and capture.
 */
enum class AudioInterfaceMode {
  Playback,
  Capture,
  Duplex,
};

/**
 * @brief Wrapper around miniaudio to deal with audio playback
 * and recording.
 */
class AudioInterface {
 public:
  AT_DISABLE_COPY_AND_MOVE_M(AudioInterface);

  /**
   * @brief Construct a new Audio Interface object.
   *
   * @param sample_rate Sample rate in Hz.
   * @param in_channels Number of input channels.
   * @param out_channels Number of output channels.
   * @param buffer_size Size of the callback buffer.
   * @param dtype Data type used for the audio interface.
   * @param mode Audio interface mode.
   * @param callback Callback function to process the audio data.
   */
  AudioInterface(uint32_t sample_rate, uint32_t in_channels,
                 uint32_t out_channels, uint32_t buffer_size,
                 AudioInterfaceDataType dtype, AudioInterfaceMode mode,
                 AudioInterfaceCallback callback);

  /**
   * @brief Destroy the Audio Interface object.
   */
  ~AudioInterface();

  /**
   * @brief Starts the audio thread.
   */
  void start();

 private:
  ma_device device;
  AudioInterfaceCallback callback;

  /**
   * @brief Callback used by miniaudio to process the audio data.
   */
  static void data_callback(ma_device* device, void* output, const void* input,
                            uint32_t n_samples);
  /**
   * @brief Get microaudio device type.
   *
   * @param mode Mode of the audio interface.
   * @return Microaudio device type.
   */
  ma_device_type get_device_type(AudioInterfaceMode mode);

  /**
   * @brief Get Microaudio data format.
   *
   * @param dtype Data type used for the audio interface.
   * @return Microaudio data format.
   */
  ma_format get_data_format(AudioInterfaceDataType dtype);
};

}  // namespace audio_toolbox

#endif  // _AUDIO_TOOLBOX_AUDIO_INTERFACE_H_