/*
 Name:		MidiPedalBoard.ino
 Author:	Patrice GODARD

 * Default Mode: Simple MIDI PedalBoard using MIDI_CHANNEL to send note on/off events from MIDI_BASE_NOTE
 * "Sample Sequencer" Mode: Press switch #2 for 3s to enter Sample Sequencer mode (led #2 is on):
      In this mode a press of Switch #1 sends a note on event from MIDI_BASE_NOTE then increment the midi note at each press
	  If DO_NOT_SEND_NOTE_OFF is defined, no note off event is sent (usefull to play samples fully for instance)
	  Switch #2 is used to stop any note/sample playing (it sends an "all notes off" MIDI message)
	  Switch #3 is used to reset the current note played by Switch #1 to MIDI_BASE_NOTE (led #3 blinks when Switch #3 is pressed)
*/
#include <MIDIUSB.h>
#include <Bounce2.h>

#define BOUNCE_LOCK_OUT
#define BOUNCE_INTERVAL 3

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
#define DO_NOT_SEND_NOTE_OFF //define this to DISABLE note off messages (useful to sequence samples and read them fully for intance)

#define CC_ALL_SOUND_OFF 120

Bounce db1 = Bounce();
Bounce db2 = Bounce();
Bounce db3 = Bounce();
Bounce db4 = Bounce();
Bounce db5 = Bounce();

void setup() {
	pinMode(PIN_IN_1, INPUT_PULLUP);
	pinMode(PIN_IN_2, INPUT_PULLUP);
	pinMode(PIN_IN_3, INPUT_PULLUP);
	pinMode(PIN_IN_4, INPUT_PULLUP);
	pinMode(PIN_IN_5, INPUT_PULLUP);


	db1.attach(PIN_IN_1);
	db1.interval(BOUNCE_INTERVAL);
	db2.attach(PIN_IN_2);
	db2.interval(BOUNCE_INTERVAL);
	db3.attach(PIN_IN_3);
	db3.interval(BOUNCE_INTERVAL);
	db4.attach(PIN_IN_4);
	db4.interval(BOUNCE_INTERVAL);
	db5.attach(PIN_IN_5);
	db5.interval(BOUNCE_INTERVAL);

	pinMode(PIN_LED_1, OUTPUT);
	pinMode(PIN_LED_2, OUTPUT);
	pinMode(PIN_LED_3, OUTPUT);
	pinMode(PIN_LED_4, OUTPUT);
	pinMode(PIN_LED_5, OUTPUT);
}

void sendNoteOn(byte note) {
	midiEventPacket_t noteOn = { 0x09, 0x90 | MIDI_CHANNEL, note, MIDI_VEL };
	MidiUSB.sendMIDI(noteOn);
	MidiUSB.flush();
}

void sendNoteOff(byte note) {
#ifndef DO_NOT_SEND_NOTE_OFF
	midiEventPacket_t noteOff = { 0x08, 0x80 | MIDI_CHANNEL, note, MIDI_VEL };
	MidiUSB.sendMIDI(noteOff);
	MidiUSB.flush();
#endif
}

void sendCC(byte ccNumber, byte ccValue) {
	midiEventPacket_t event = { 0x0B, 0xB0 | MIDI_CHANNEL, ccNumber, ccValue };
	MidiUSB.sendMIDI(event);
	MidiUSB.flush();
}

void noteOnOff(byte channel, byte pitch, byte velocity, byte state) {
	if (channel = MIDI_CHANNEL) {
		int led = pitch - MIDI_BASE_NOTE + PIN_LED_1;
		if (led >= PIN_LED_1 && led <= PIN_LED_5) {
			digitalWrite(led, state);
		}
	}
}

void loop() {
	db1.update();
	db2.update();

	if (db1.fell()) {
		sendNoteOn(MIDI_BASE_NOTE);
	}
	if (db1.rose()) {
		sendNoteOff(MIDI_BASE_NOTE);
	}
	if (db2.fell()) {
		sendNoteOn(MIDI_BASE_NOTE + 1);
	}
	if (db2.rose()) {
		sendNoteOff(MIDI_BASE_NOTE + 1);
	}
	if (db3.fell()) {
		sendNoteOn(MIDI_BASE_NOTE + 2);
	}
	if (db3.rose()) {
		sendNoteOff(MIDI_BASE_NOTE + 2);
	}
	if (db4.fell()) {
		sendNoteOn(MIDI_BASE_NOTE + 3);
	}
	if (db4.rose()) {
		sendNoteOff(MIDI_BASE_NOTE + 3);
	}
	if (db5.fell()) {
		sendNoteOn(MIDI_BASE_NOTE + 4);
	}
	if (db5.rose()) {
		sendNoteOff(MIDI_BASE_NOTE + 4);
	}

	midiEventPacket_t rx = MidiUSB.read();
	switch (rx.header) {
	case 0:
		break; //No pending events

	case 0x9:
		noteOnOff(
			rx.byte1 & 0xF,  //channel
			rx.byte2,        //pitch
			rx.byte3,         //velocity
			HIGH
		);
		break;

	case 0x8:
		noteOnOff(
			rx.byte1 & 0xF,  //channel
			rx.byte2,        //pitch
			rx.byte3,         //velocity
			LOW
		);
		break;
		/*
			case 0xB:

				controlChange(
					rx.byte1 & 0xF,  //channel
					rx.byte2,        //control
					rx.byte3         //value
				);
				break;
				*/
	}
}
