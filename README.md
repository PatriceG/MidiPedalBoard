# MidiPedalBoard
Simple MIDI Pedalboard with an optional "Sample sequencer" mode - for Arduino Leonardo or Pro Micro

# Modes of operation

## Default Mode
Simple MIDI PedalBoard using MIDI_CHANNEL to send note on/off events from MIDI_BASE_NOTE
 
## "Sample Sequencer" Mode
Press switch #2 for 3s to enter Sample Sequencer mode (led #2 is on):

In this mode a press of Switch #1 sends a note-on event from MIDI_BASE_NOTE then increment the midi note at each press;

If DO_NOT_SEND_NOTE_OFF is defined, no note-off event is sent (usefull to play samples fully for instance)

Switch #2 is used to stop any note/sample playing (it sends an "all notes off" MIDI message)

Switch #3 is used to reset the current note played by 

Switch #1 to MIDI_BASE_NOTE (led #3 blinks when Switch #3 is pressed)
