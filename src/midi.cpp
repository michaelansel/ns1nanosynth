#include "midi.h"

void midiHook()
{
    midiEventPacket_t e;

    e = MidiUSB.read();
    if (e.header != 0)
    {
        // Serial.println("new midi message received");
        handleMidiEvent(e);
    }
}