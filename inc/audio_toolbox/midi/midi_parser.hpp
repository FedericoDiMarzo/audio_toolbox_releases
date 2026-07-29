#pragma once

/*
  Taken from https://github.com/electro-smith/libDaisy
*/

#include <cstdint>
#include <cstdlib>
#include <vector>

#include "audio_toolbox/midi/midi_token.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{
/**
 * @brief   Utility class for parsing raw byte streams into MIDI messages
 * @details Implemented as a state machine designed to parse one byte at a time
 */
class MidiParser
{
  public:
    AT_DISABLE_COPY_AND_MOVE_M(MidiParser);

    MidiParser()
    {
        reset();
    }

    /**
     * @brief Parse one MIDI byte. If the byte completes a parsed event,
     * its value will be assigned to the dereferenced output pointer.
     * Otherwise, status is preserved in anticipation of the next
     * sequential byte. Return value indicates if a new event was parsed or
     * not.
     *
     * @param byte Raw MIDI byte to parse
     * @param event_out Pointer to output event object, value assigned on parse
     * success
     * @return true If a new event was parsed
     * @return false If no new event was parsed
     */
    bool parse(uint8_t byte, MidiToken* event_out);

    /**
     * @tparam C Container type for uint8_t.
     * @param bytes Container of raw MIDI bytes to parse.
     * @param tokens Vector of parsed MIDI tokens.
     * @return True if at least one event was parsed.
     */
    template <typename C>
    bool parse(C& bytes, std::vector<MidiToken>& tokens)
    {
        for (auto byte : bytes)
        {
            MidiToken token;
            if (parse(byte, &token))
            {
                tokens.push_back(token);
            }
        }
        return tokens.size() > 0;
    }

    /**
     * @brief Reset parser to default state
     */
    void reset();

  private:
    enum ParserState
    {
        ParserEmpty,
        ParserHasStatus,
        ParserHasData0,
        ParserSysEx,
    };

    ParserState pstate_;
    MidiToken incoming_message_;
    MidiStatusType running_status_;

    // Masks to check for message type, and byte content
    const uint8_t kStatusByteMask = 0x80;
    const uint8_t kMessageMask = 0x70;
    const uint8_t kDataByteMask = 0x7F;
    const uint8_t kChannelMask = 0x0F;
    const uint8_t kSystemRealTimeMask = 0x07;
};

} // namespace audio_toolbox
