#include "audio_toolbox/midi/midi.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class MidiTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        midi_parser.parse(midi_msg, midi_tokens);
        token_0 = midi_tokens[0];
        cc_token = midi_tokens[1];
    }

    void TearDown() override {}

    uint8_t status_byte = 0x90;
    uint8_t data_byte = 0x45;
    uint8_t cc_status_byte = 0xB7;
    uint8_t cc_number = 84;
    uint8_t cc_value = 121;
    std::vector<uint8_t> midi_msg = { status_byte, data_byte, data_byte, cc_status_byte, cc_number, cc_value };

    MidiToken token_0;
    MidiToken cc_token;
    MidiCCParameter cc_param{ cc_number, 0, 127 };
    MidiParser midi_parser;
    std::vector<MidiToken> midi_tokens;
};

TEST_F(MidiTest, MidiCCParameterGetStart)
{
    EXPECT_EQ(cc_param.get(), 0);
}

TEST_F(MidiTest, MidiCCParameterUpdate)
{
    cc_param.update(midi_tokens);
    EXPECT_EQ(cc_param.get(), cc_value);
}
