#include "audio_toolbox/core/audio_interface.hpp"

#include <cstring>
#include <stdexcept>
#include <vector>

using namespace audio_toolbox;

/**
 * @brief Points to the callback that has been set in
 * the AudioInterface.
 */
AudioInterfaceCallback callback_static;

AudioInterface::AudioInterface(uint32_t sample_rate, uint32_t in_channels,
                               uint32_t out_channels, uint32_t buffer_size,
                               AudioInterfaceDataType dtype,
                               AudioInterfaceMode mode,
                               AudioInterfaceCallback callback)
    : callback{callback} {
  // Saving the callback statically
  callback_static = callback;

  // Setting up Miniaudio
  ma_format format = get_data_format(dtype);
  ma_device_type ma_mode = get_device_type(mode);
  ma_device_config config = ma_device_config_init(ma_mode);
  config.sampleRate = sample_rate;
  config.capture.format = format;
  config.playback.format = format;
  config.capture.channels = in_channels;
  config.playback.channels = out_channels;
  config.periodSizeInFrames = buffer_size;
  config.dataCallback = data_callback;

  // config.pUserData = pMyCustomData;
  ma_result rs = ma_device_init(NULL, &config, &device);
  if (rs != MA_SUCCESS)
    AT_THROW_M(std::runtime_error,
               "AudioInteface device initialization failed.");
}

audio_toolbox::AudioInterface::~AudioInterface() { ma_device_uninit(&device); }

void audio_toolbox::AudioInterface::start() {
#ifdef AT_ENABLE_DEBUGGING
  // When the debug mode is enabled the processing loop
  // is looped in the main thread
  std::vector<real_type> input(buffer_size);
  std::vector<real_type> output(buffer_size);
  while (true) {
    callback_static(input.data(), output.data(), buffer_size);
  }
#else
  ma_device_start(&device);
#endif
}

void AudioInterface::data_callback([[maybe_unused]] ma_device* device,
                                   void* output, const void* input,
                                   uint32_t n_samples) {
  callback_static(static_cast<const real_type*>(input),
                  static_cast<real_type*>(output), n_samples);
}

ma_device_type audio_toolbox::AudioInterface::get_device_type(
    AudioInterfaceMode mode) {
  switch (mode) {
    case AudioInterfaceMode::Playback:
      return ma_device_type_playback;
    case AudioInterfaceMode::Capture:
      return ma_device_type_capture;
    case AudioInterfaceMode::Duplex:
      return ma_device_type_duplex;
    default:

      AT_THROW_M(std::invalid_argument, "Unsupported mode");
  }
}

ma_format audio_toolbox::AudioInterface::get_data_format(
    AudioInterfaceDataType dtype) {
  switch (dtype) {
    case AudioInterfaceDataType::float32:
      return ma_format_f32;
    // case AudioInterfaceDataType::INT16:
    //   return ma_format_s16;
    // case AudioInterfaceDataType::INT32:
    //   return ma_format_s32;
    // case AudioInterfaceDataType::UINT8:
    //   return ma_format_u8;
    default:
      AT_THROW_M(std::invalid_argument, "Unsupported dtype");
  }
}

uint32_t audio_toolbox::read_audio_file(std::string filename, real_type* buffer,
                                        uint32_t max_len, uint32_t n_channels,
                                        uint32_t sample_rate) {
  std::string error_msg = "Audio file read failed";

  // Setting up the decoder
  ma_decoder decoder;
  ma_decoder_config config =
      ma_decoder_config_init(ma_format_f32, n_channels, sample_rate);
  ma_result rs = ma_decoder_init_file(filename.c_str(), &config, &decoder);
  if (rs != MA_SUCCESS) AT_THROW_M(std::runtime_error, error_msg);

  // Reading the audio file
  ma_uint64 frames_read;
  rs = ma_decoder_read_pcm_frames(&decoder, buffer, max_len, &frames_read);
  if (rs != MA_SUCCESS) AT_THROW_M(std::runtime_error, error_msg);

  ma_decoder_uninit(&decoder);

  return static_cast<uint32_t>(frames_read);
}

void audio_toolbox::write_audio_file(std::string filename,
                                     const real_type* buffer,
                                     uint32_t n_samples, uint32_t n_channels,
                                     uint32_t sample_rate) {
  std::string error_msg = "Audio file write failed";

  // Setting up the encoder
  ma_encoder_config config = ma_encoder_config_init(
      ma_encoding_format_wav, ma_format_f32, n_channels, sample_rate);
  ma_encoder encoder;
  ma_result rs = ma_encoder_init_file(filename.c_str(), &config, &encoder);
  if (rs != MA_SUCCESS) AT_THROW_M(std::runtime_error, error_msg);

  // Saving the audio file
  ma_uint64 frames_written;
  rs =
      ma_encoder_write_pcm_frames(&encoder, buffer, n_samples, &frames_written);
  if (rs != MA_SUCCESS) AT_THROW_M(std::runtime_error, error_msg);

  ma_encoder_uninit(&encoder);
}
