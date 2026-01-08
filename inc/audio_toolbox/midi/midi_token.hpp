#ifndef _AUDIO_TOOLBOX_MIDI_TOKEN_H_
#define _AUDIO_TOOLBOX_MIDI_TOKEN_H_

/*
  Adapted from https://github.com/electro-smith/libDaisy

  Known issues:
  - SYSEX_BUFFER_LEN cannot be changed.
  - Only 3-byte messages are correctly parsed into MidiTokens.

*/

#include <cstdint>

#define SYSEX_BUFFER_LEN 128

namespace audio_toolbox {

/**
 * @brief Enum containing all possible MIDI message types.
 */
enum class MidiStatusType {
  NoteOff,
  NoteOn,
  PolyphonicKeyPressure,
  ControlChange,
  ProgramChange,
  ChannelPressure,
  PitchBend,
  SystemCommon,
  SystemRealTime,
  ChannelMode,
  MessageLast,
};

/**
 * @brief Enum containing all possible SystemCommon message types.
 */
enum class SystemCommonType {
  SystemExclusive,
  MTCQuarterFrame,
  SongPositionPointer,
  SongSelect,
  SCUndefined0,
  SCUndefined1,
  TuneRequest,
  SysExEnd,
  SystemCommonLast,
};

/**
 * @brief Enum containing all possible SystemRealTime message types.
 */
enum SystemRealTimeType {
  TimingClock,
  SRTUndefined0,
  Start,
  Continue,
  Stop,
  SRTUndefined1,
  ActiveSensing,
  Reset,
  SystemRealTimeLast,
};

/**
 * @brief Enum containing all possible ChannelMode message types.
 */
enum ChannelModeType {
  AllSoundOff,
  ResetAllControllers,
  LocalControl,
  AllNotesOff,
  OmniModeOff,
  OmniModeOn,
  MonoModeOn,
  PolyModeOn,
  ChannelModeLast,
};

/**
 * @brief Struct containing note, and velocity data for a given channel.
 * Can be made from MidiToken
 */
struct NoteOffEvent {
  int32_t channel;
  uint8_t note;
  uint8_t velocity;
};

/**
 * @brief Struct containing note, and velocity data for a given channel.
 * Can be made from MidiToken
 */
struct NoteOnEvent {
  int32_t channel;
  uint8_t note;
  uint8_t velocity;
};

/**
 * @brief Struct containing note, and pressure data for a given channel.
 * Can be made from MidiToken
 */
struct PolyphonicKeyPressureEvent {
  int32_t channel;
  uint8_t note;
  uint8_t pressure;
};

/**
 * @brief Struct containing control number, and value for a given channel.
 * Can be made from MidiToken
 */
struct ControlChangeEvent {
  int32_t channel;
  uint8_t control_number;
  uint8_t value;
};

/**
 * @brief Struct containing new program number, for a given channel.
 * Can be made from MidiToken
 */
struct ProgramChangeEvent {
  int32_t channel;
  uint8_t program;
};

/**
 * @brief Struct containing pressure (aftertouch), for a given channel.
 * Can be made from MidiToken
 */
struct ChannelPressureEvent {
  int32_t channel;
  uint8_t pressure;
};

/**
 * @brief Struct containing pitch bend value for a given channel.
 * Can be made from MidiToken
 */
struct PitchBendEvent {
  int32_t channel;
  int16_t value;
};
/**
 * @brief Struct containing channel mode event for a given channel.
 * Can be made from MidiToken
 */
struct ChannelModeEvent {
  int32_t channel;
  ChannelModeType event_type;
  int16_t value;
};

/**
 * @brief Struct containing sysex data.
 * Can be made from MidiToken
 */
struct SystemExclusiveEvent {
  int32_t length;
  uint8_t data[SYSEX_BUFFER_LEN];
};

/**
 * @brief Struct containing QuarterFrame data.
 * Can be made from MidiToken
 */
struct MTCQuarterFrameEvent {
  uint8_t message_type;
  uint8_t value;
};

/**
 * @brief Struct containing song position data.
 * Can be made from MidiToken
 */
struct SongPositionPointerEvent {
  uint16_t position;
};

/**
 * @brief Struct containing song select data.
 * Can be made from MidiToken
 */
struct SongSelectEvent {
  uint8_t song;
};

/**
 * @brief Struct containing sound off data.
 * Can be made from MidiToken
 */
struct AllSoundOffEvent {
  int32_t channel;
};
/**
 * @brief Struct containing ResetAllControllersEvent data.
 * Can be made from MidiToken
 */
struct ResetAllControllersEvent {
  int32_t channel;
  uint8_t value;
};

/**
 * @brief Struct containing LocalControlEvent data.
 * Can be made from MidiToken
 */
struct LocalControlEvent {
  int32_t channel;
  bool local_control_off;
  bool local_control_on;
};

/**
 * @brief Struct containing AllNotesOffEvent data.
 * Can be made from MidiToken
 */
struct AllNotesOffEvent {
  int32_t channel;
};
/**
 * @brief Struct containing OmniModeOffEvent data.
 * Can be made from MidiToken
 */
struct OmniModeOffEvent {
  int32_t channel;
};

/**
 * @brief Struct containing OmniModeOnEvent data.
 * Can be made from MidiToken
 */
struct OmniModeOnEvent {
  int32_t channel;
};

/**
 * @brief Struct containing MonoModeOnEvent data.
 * Can be made from MidiToken
 */
struct MonoModeOnEvent {
  int32_t channel;
  uint8_t num_channels;
};

/**
 * @brief Struct containing PolyModeOnEvent data.
 * Can be made from MidiToken
 */
struct PolyModeOnEvent {
  int32_t channel;
};

/**
 * @brief Simple MidiToken with message type, channel, and data[2] members.
 */
struct MidiToken {
  MidiStatusType type;
  int32_t channel;
  uint8_t data[2];
  uint8_t sysex_data[SYSEX_BUFFER_LEN];
  uint8_t sysex_message_len;
  SystemCommonType sc_type;
  SystemRealTimeType srt_type;
  ChannelModeType cm_type;

  /**
   * @brief Returns the data within the MidiToken as a NoteOffEvent struct.
   * @return A NoteOffEvent struct with the same data as this MidiToken.
   */
  NoteOffEvent as_note_off() const {
    NoteOffEvent m;
    m.channel = channel;
    m.note = data[0];
    m.velocity = data[1];
    return m;
  }

  /**
   * @brief Returns the data within the MidiToken as a NoteOnEvent struct.
   * @return A NoteOnEvent struct with the same data as this MidiToken.
   */
  NoteOnEvent as_note_on() const {
    NoteOnEvent m;
    m.channel = channel;
    m.note = data[0];
    m.velocity = data[1];
    return m;
  }

  /**
   * @brief Returns the data within the MidiToken as a
   * PolyphonicKeyPressureEvent struct
   */
  PolyphonicKeyPressureEvent as_polyphonic_key_pressure() const {
    PolyphonicKeyPressureEvent m;
    m.channel = channel;
    m.note = data[0];
    m.pressure = data[1];
    return m;
  }

  /**
   * @brief Returns the data within the MidiToken as a ControlChangeEvent
   * struct.
   */
  ControlChangeEvent as_control_change() const {
    ControlChangeEvent m;
    m.channel = channel;
    m.control_number = data[0];
    m.value = data[1];
    return m;
  }

  /**
   * @brief Returns the data within the MidiToken as a ProgramChangeEvent
   * struct.
   */
  ProgramChangeEvent as_program_change() const {
    ProgramChangeEvent m;
    m.channel = channel;
    m.program = data[0];
    return m;
  }

  /**
   * @brief Returns the data within the MidiToken as a ProgramChangeEvent
   * struct.
   */
  ChannelPressureEvent as_channel_pressure() const {
    ChannelPressureEvent m;
    m.channel = channel;
    m.pressure = data[0];
    return m;
  }

  /**
   * @brief Returns the data within the MidiToken as a PitchBendEvent struct.
   */
  PitchBendEvent as_pitch_bend() const {
    PitchBendEvent m;
    m.channel = channel;
    m.value = ((uint16_t)data[1] << 7) + (data[0] - 8192);
    return m;
  }

  ChannelModeEvent as_channel_mode() const {
    ChannelModeEvent m;

    m.channel = channel;
    m.event_type = (ChannelModeType)(data[0] - 120);
    m.value = data[1];

    return m;
  }

  SystemExclusiveEvent as_system_exclusive() const {
    SystemExclusiveEvent m;
    m.length = sysex_message_len;
    for (int32_t i = 0; i < SYSEX_BUFFER_LEN; i++) {
      m.data[i] = 0;
      if (i < m.length) {
        m.data[i] = sysex_data[i];
      }
    }
    return m;
  }
  MTCQuarterFrameEvent as_mtc_quarter_frame() const {
    MTCQuarterFrameEvent m;
    m.message_type = (data[0] & 0x70) >> 4;
    m.value = data[0] & 0x0f;
    return m;
  }
  SongPositionPointerEvent as_song_position_pointer() const {
    SongPositionPointerEvent m;
    m.position = ((uint16_t)data[1] << 7) | data[0];
    return m;
  }
  SongSelectEvent AsSongSelect() const {
    SongSelectEvent m;
    m.song = data[0];
    return m;
  }
  AllSoundOffEvent as_all_sound_off() const {
    AllSoundOffEvent m;
    m.channel = channel;
    return m;
  }
  ResetAllControllersEvent as_reset_all_controllers() const {
    ResetAllControllersEvent m;
    m.channel = channel;
    m.value = data[1];
    return m;
  }
  LocalControlEvent as_local_control() const {
    LocalControlEvent m;
    m.channel = channel;
    m.local_control_off = data[1] == 0;
    m.local_control_on = data[1] == 127;
    return m;
  }
  AllNotesOffEvent as_all_notes_off() const {
    AllNotesOffEvent m;
    m.channel = channel;
    return m;
  }
  OmniModeOffEvent as_omni_mode_off() const {
    OmniModeOffEvent m;
    m.channel = channel;
    return m;
  }
  OmniModeOnEvent as_omni_mode_on() const {
    OmniModeOnEvent m;
    m.channel = channel;
    return m;
  }
  MonoModeOnEvent as_mono_mode_on() const {
    MonoModeOnEvent m;
    m.channel = channel;
    m.num_channels = data[1];
    return m;
  }
  PolyModeOnEvent as_poly_mode_on() const {
    PolyModeOnEvent m;
    m.channel = channel;
    return m;
  }
};

}  // namespace audio_toolbox

#endif  // _AUDIO_TOOLBOX_MIDI_TOKEN_H_