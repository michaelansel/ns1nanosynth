#include "arturia_minilabmk2.h"

byte padHardwareIdToNumber(byte padHardwareId)
{
    if (padHardwareId >= MINILAB_PAD_BASE && padHardwareId < (MINILAB_PAD_BASE + MINILAB_PAD_COUNT))
    {
        return padHardwareId - MINILAB_PAD_BASE + 1; // 1-16
    }
    else
    {
        return 0;
    }
}

byte padNumberToHardwareId(byte padNumber)
{
    if (padNumber > 0 && padNumber <= MINILAB_PAD_COUNT)
    {
        return MINILAB_PAD_BASE + padNumber - 1; // 1-16
    }
    else
    {
        return MINILAB_PAD_BASE;
    }
}

void setPadColor(byte padNumber, byte color)
{
    setButtonState(padNumberToHardwareId(padNumber), color);
}

void setButtonState(byte buttonHardwareId, byte color)
{
    byte msg[] = {0xF0, 0x00, 0x20, 0x6B, 0x7F, 0x42, 0x02, 0x00, 0x10, 0x70, 0x7F, 0xF7};
    msg[9] = buttonHardwareId; // buttonHardwareId
    // TODO only pads support colors
    msg[10] = color; // buttonColor
    sendSysEx(msg, sizeof(msg) / sizeof(byte));
}

bool sysExInProgress = false;
byte buffer[32];
uint8_t bufferPosition = 0;
void inline addToBuffer(byte b)
{
    if (bufferPosition < (sizeof(buffer) / sizeof(byte)))
    {
        buffer[bufferPosition++] = b;
    }
    else
    {
        // Swallow overflow
        Serial.println("addToBuffer Overflow");
    }
}

void inline endSysEx()
{
    const byte expectedPrefix[] = {0xF0, 0x00, 0x20, 0x6B, 0x7F, 0x42, 0x02, 0x00};
    if (bufferPosition == 12 && memcmp(expectedPrefix, buffer, 8) == 0)
    {
        switch (buffer[8])
        {
        case 0x00:
            // Keypress
            minilabEvent_t e;
            e.button = buffer[9];
            e.isKeyDown = buffer[10] > 0;
            handleMinilabEvent(e);
            break;
        default:
            Serial.print("Unrecognized MiniLab message ");
            for (int i = 0; i < bufferPosition; i++)
            {
                Serial.print(buffer[i], 16);
                Serial.print(" ");
            }
            Serial.println();
            break;
        }
    }
    else
    {
        Serial.print("Unrecognized SysEx message ");
        for (int i = 0; i < bufferPosition; i++)
        {
            Serial.print(buffer[i], 16);
            Serial.print(" ");
        }
        Serial.println();
    }
    sysExInProgress = false;
    bufferPosition = 0;
}

bool handleAsMinilabEvent(midiEventPacket_t e)
{
    switch (e.header)
    {
    case MIDI_SYSEX:
        sysExInProgress = true;
        addToBuffer(e.byte1);
        addToBuffer(e.byte2);
        addToBuffer(e.byte3);
        return true;
        break;
    case MIDI_SYSEX_END_1:
        addToBuffer(e.byte1);
        endSysEx();
        return true;
        break;
    case MIDI_SYSEX_END_2:
        addToBuffer(e.byte1);
        addToBuffer(e.byte2);
        endSysEx();
        return true;
        break;
    case MIDI_SYSEX_END_3:
        addToBuffer(e.byte1);
        addToBuffer(e.byte2);
        addToBuffer(e.byte3);
        endSysEx();
        return true;
        break;
    default:
        sysExInProgress = false;
        bufferPosition = 0;
        return false;
        break;
    }
}