#ifndef _AUDIO_TOOLBOX_MIDI_H_
#define _AUDIO_TOOLBOX_MIDI_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/midi/midi_parser.hpp"
#include "audio_toolbox/midi/midi_token.hpp"

using namespace audio_toolbox;
namespace audio_toolbox {

/**
 * @brief Check if a MIDI event is of a certain type.
 *
 * @param event MIDI event.
 * @param type Type to check.
 * @return True if the event is of the given type, false otherwise.
 */
inline bool midi_event_is_type(const MidiToken& event, MidiStatusType type);

/**
 * @brief Check if a MIDI event is from a certain channel.
 *
 * @param event MIDI event.
 * @param channel Channel to check.
 * @return True if the event is from the given channel, false otherwise.
 */
inline bool midi_event_is_from_channel(const MidiToken& event, uint8_t channel);

/**
 * @brief Check if a MIDI event is from a certain CC number.
 *
 * @param event MIDI event.
 * @param cc_number CC number to check.
 * @return True if the event is from the given CC number, false otherwise.
 */
inline bool midi_event_is_from_cc(MidiToken& event, uint8_t cc_number);

/**
 * @brief Read a MIDI CC message from a MIDI queue.
 */
// TODO: LOGARIHTMIC SCALING

class MidiCCParameter {
  real_type min, max;
  uint8_t cc_number;
  uint8_t new_value;
  uint8_t last_value;

 public:
  AT_DISABLE_COPY_AND_MOVE_M(MidiCCParameter);

  /**
   * @brief Construct a new Midi C C Parameter object.
   *
   * @param cc_number CC number to be read.
   * @param min Minimum value mapped to the parameter.
   * @param max Maximum value mapped to the parameter.
   */
  MidiCCParameter(uint8_t cc_number, real_type min, real_type max);

  /**
   * @brief Update the parameter value with the latest CC message.
   *
   * @tparam C MIDI tokens container of MidiTokens.
   * @param midi_tokens Current MIDI tokens.
   */
  template <typename C>
  inline void update(C& midi_tokens);

  /**
   * @brief Get the value of the parameter.
   *
   * @return Value of the parameter.
   */
  inline real_type get();
};

using MidiCCDispatcher = UpdateDispatcher<MidiCCParameter>;

}  // namespace audio_toolbox

#include "audio_toolbox/midi/midi_imp.hpp"

#endif  // _AUDIO_TOOLBOX_MIDI_H_