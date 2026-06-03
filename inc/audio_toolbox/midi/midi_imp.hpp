namespace audio_toolbox
{

bool midi_event_is_type(const MidiToken& event, MidiStatusType type)
{
    return event.type == type;
}

bool midi_event_is_from_channel(const MidiToken& event, uint8_t channel)
{
    return event.channel == channel;
}

bool midi_event_is_from_cc(MidiToken& event, uint8_t cc_number)
{
    return midi_event_is_type(event, MidiStatusType::ControlChange) &&
           event.as_control_change().control_number == cc_number;
}

MidiCCParameter::MidiCCParameter(uint8_t cc_number, real_type min, real_type max)
    : min(min),
      max(max),
      cc_number(cc_number),
      new_value(0),
      last_value(0){};

template <typename C>
void MidiCCParameter::update(C& midi_tokens)
{
    last_value = new_value;
    // Traversing the collection in reverse order to get the latest value.
    auto itr = std::find_if(std::rbegin(midi_tokens),
                            std::rend(midi_tokens),
                            [this](MidiToken& e) { return midi_event_is_from_cc(e, cc_number); });
    if (itr != std::rend(midi_tokens))
    {
        // Update the value if the CC message is found.
        new_value = itr->data[1];
    }
}

real_type MidiCCParameter::get()
{
    return linear_map(new_value, 0, 127, min, max);
};

} // namespace audio_toolbox