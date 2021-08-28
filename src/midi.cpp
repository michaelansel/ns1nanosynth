#include "midi.h"

void midiHook() {
    midiEventPacket_t e;

    if (MidiUSB.available() > 0) {
        // Serial.println("new midi message received");
        e = MidiUSB.read();
        handleMidiEvent(e);
    }
}