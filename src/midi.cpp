#include "midi.h"

void midiHook()
{
    midiEventPacket_t e;

    e = MidiUSB.read();
    if (e.header != 0)
    {
        // Serial.print("midi message received ");
        // Serial.print(e.header, 16);
        // Serial.print(" ");
        // Serial.print(e.byte1, 16);
        // Serial.print(" ");
        // Serial.print(e.byte2, 16);
        // Serial.print(" ");
        // Serial.println(e.byte3, 16);
        handleMidiEvent(e);
    }
}

void sendSysEx(byte *data, byte length)
{
    byte position = 0;
    byte buffer[4];
    byte bufferLength;
    while (position < length)
    {
        if ((length - position) > 3)
        {
            buffer[0] = MIDI_SYSEX;
            buffer[1] = data[position++];
            buffer[2] = data[position++];
            buffer[3] = data[position++];
            bufferLength = 4;
        }
        else
        {
            switch (length - position)
            {
            case 1:
                buffer[0] = MIDI_SYSEX_END_1;
                buffer[1] = data[position++];
                bufferLength = 2;
                break;
            case 2:
                buffer[0] = MIDI_SYSEX_END_2;
                buffer[1] = data[position++];
                buffer[2] = data[position++];
                bufferLength = 3;
                break;
            case 3:
                buffer[0] = MIDI_SYSEX_END_3;
                buffer[1] = data[position++];
                buffer[2] = data[position++];
                buffer[3] = data[position++];
                bufferLength = 4;
                break;
            }
        }
        MidiUSB.write(buffer, bufferLength);
    }
    MidiUSB.flush();
}