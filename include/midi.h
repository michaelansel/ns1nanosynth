#include "main.h"
#include "MIDIUSB.h"

const byte MIDI_NOTEON = 0x09;
const byte MIDI_NOTEOFF = 0x08;
const byte MIDI_CC = 0x0B;
const byte MIDI_PB = 0x0E; // pitch bend

void handleMidiEvent(midiEventPacket_t e);
void midiHook();