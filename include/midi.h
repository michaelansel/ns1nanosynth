#include "main.h"
#include "MIDIUSB.h"

// USB-MIDI Code Index Numbers
// pg16-17 https://www.usb.org/sites/default/files/midi10.pdf
const byte MIDI_NOTEON = 0x09;
const byte MIDI_NOTEOFF = 0x08;
const byte MIDI_CC = 0x0B; // control change
const byte MIDI_PB = 0x0E; // pitch bend
const byte MIDI_SYSEX = 0x04; // start/continue
const byte MIDI_SYSEX_END_1 = 0x5; // one more byte
const byte MIDI_SYSEX_END_2 = 0x6; // two more bytes
const byte MIDI_SYSEX_END_3 = 0x7; // three more bytes

void handleMidiEvent(midiEventPacket_t e);
void midiHook();
void sendSysEx(byte *data, byte length);