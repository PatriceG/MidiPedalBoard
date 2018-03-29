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

# Configuration

PIN Mappings and MIDI settings are configured via the following constants:

    
```C
//PIN mappings
#define PIN_IN_1 2
#define PIN_IN_2 3
#define PIN_IN_3 4
#define PIN_IN_4 5
#define PIN_IN_5 6

#define PIN_LED_1 6
#define PIN_LED_2 7
#define PIN_LED_3 8
#define PIN_LED_4 9
#define PIN_LED_5 10

//MIDI settings
#define MIDI_VEL 127
#define MIDI_CHANNEL 0  //midi channel 1!
#define MIDI_BASE_NOTE 48
#define DO_NOT_SEND_NOTE_OFF //define this to DISABLE note off messages (useful to sequence samples and read them fully for intance)`
```

