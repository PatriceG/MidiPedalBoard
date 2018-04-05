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

#include <pitchToNote.h>
#include <pitchToFrequency.h>
#include <MIDIUSB.h>
#include <frequencyToNote.h>
#include <Bounce2.h>


#define BOUNCE_LOCK_OUT
#define BOUNCE_INTERVAL 3

#define PIN_IN_1 2
#define PIN_IN_2 3
#define PIN_IN_3 4
#define PIN_IN_4 5
#define PIN_IN_5 6

#define PIN_LED_1 7
#define PIN_LED_2 8
#define PIN_LED_3 9
#define PIN_LED_4 10
#define PIN_LED_5 16

#define MIDI_VEL 127
#define MIDI_CHANNEL 0  //midi channel 1!
#define MIDI_BASE_NOTE 48
#define DO_NOT_SEND_NOTE_OFF //define this to DISABLE note off messages in INCREMENT_NOTE mode (useful to sequence samples and read them fully for intance)

#define CC_ALL_SOUND_OFF 120

#define MODE_CHANGE_DELAY 3000

uint8_t currentNote = MIDI_BASE_NOTE;

uint8_t doNotSendNoteOff = false;
enum mode {NORMAL, INCREMENT_NOTE};
mode currentMode = NORMAL;
int t0=0; //used to handle mode changes

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

#ifdef DO_NOT_SEND_NOTE_OFF
  doNotSendNoteOff = true;
#endif
}

void sendNoteOn(byte note) {
	midiEventPacket_t noteOn = { 0x09, 0x90 | MIDI_CHANNEL, note, MIDI_VEL };
	MidiUSB.sendMIDI(noteOn);
	MidiUSB.flush();
}

void sendNoteOff(byte note) {
  if(currentMode == NORMAL || (currentMode == INCREMENT_NOTE && doNotSendNoteOff == false)){
    midiEventPacket_t noteOff = { 0x08, 0x80 | MIDI_CHANNEL, note, MIDI_VEL };
    MidiUSB.sendMIDI(noteOff);
    MidiUSB.flush();
  }
}

void sendCC(byte ccNumber, byte ccValue) {
	midiEventPacket_t event = { 0x0B, 0xB0 | MIDI_CHANNEL, ccNumber, ccValue };
	MidiUSB.sendMIDI(event);
	MidiUSB.flush();
}

void noteOnOff(byte channel, byte pitch, byte velocity, byte state) {
	if (channel = MIDI_CHANNEL+1) {
		int led = pitch - MIDI_BASE_NOTE + PIN_LED_1;
    if (led == PIN_LED_4 +1){ //hack to get to the right pin number on the Arduino Pro Micro where there is no D11
      led = PIN_LED_5;
    }
		if (led >= PIN_LED_1 && led <= PIN_LED_5) {
			digitalWrite(led, state);
		}
	}
}

/*
* if switchstate is LOW longer than MODE_CHANGE_DELAY then switch to INCREMENT_NOTE mode
*/
void handleMode(boolean switchState) {
	if (switchState == LOW && currentMode == NORMAL) {
		int t1 = millis();
		if (t1 - t0 > MODE_CHANGE_DELAY) {
			currentMode = INCREMENT_NOTE;
			digitalWrite(PIN_LED_2, HIGH);
		}
	}
}

void loop() {
	db1.update();
	db2.update();
  db3.update();
  db4.update();
  db5.update();

	if (db1.fell()) {		
		if (currentMode == NORMAL) {
			sendNoteOn(MIDI_BASE_NOTE);
		}
		else {
			sendNoteOn(currentNote);
		}
	}
	if (db1.rose()) {
		if (currentMode == NORMAL) {
			sendNoteOff(MIDI_BASE_NOTE);
		}
		else {
			sendNoteOff(currentNote);
			currentNote++;
		}
	}
	if (db2.fell()) {
		if (currentMode == NORMAL) {
			t0 = millis();
			sendNoteOn(MIDI_BASE_NOTE + 1);
		}
		else {
			sendCC(CC_ALL_SOUND_OFF, 0);
		}
	}
	if (db2.rose()) {
		if (currentMode == NORMAL) {
			sendNoteOff(MIDI_BASE_NOTE + 1);
		}
	}
	handleMode(db2.read());
	if (db3.fell()) {
    if (currentMode == NORMAL) {
		  sendNoteOn(MIDI_BASE_NOTE + 2);
    }else{
      currentNote =  MIDI_BASE_NOTE;
      digitalWrite(PIN_LED_3, HIGH);
      delay(150);  
      digitalWrite(PIN_LED_3, LOW);
    }
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
