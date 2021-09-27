// Pads: 112 - 127 (0x70-7F) (multicolor)
// Shift: 46 (0x2E)
// PadSelect: 47 (0x2F)
// Oct-: 16 (0x10)
// Oct+: 17 (0x11)

// Set color: F0 00 20 6B 7F 42 02 00 10 ID COLOR F7
// Key down: F0 00 20 6B 7F 42 02 00 00 ID 7F F7
// Key up:   F0 00 20 6B 7F 42 02 00 00 ID 7F F7
// Select preset: F0 00 20 6B 7F 42 1B ID F7

#include "main.h"
#include "midi.h"

// Monochrome Button SysEx IDs
#define MINILAB_BUTTON_SHIFT 0x2E
#define MINILAB_BUTTON_PAD_SELECT 0x2F
#define MINILAB_BUTTON_OCTAVE_DOWN 0x10
#define MINILAB_BUTTON_OCTAVE_UP 0x11

// Multicolor Pad SysEx Base ID (pad 1)
#define MINILAB_PAD_BASE 0x70
#define MINILAB_PAD_COUNT 16

// Multicolor Pad Color Codes
#define MINILAB_BLACK 0x00
#define MINILAB_RED 0x01
#define MINILAB_GREEN 0x04
#define MINILAB_YELLOW 0x05
#define MINILAB_BLUE 0x10
#define MINILAB_MAGENTA 0x11
#define MINILAB_CYAN 0x14
#define MINILAB_WHITE 0x7F

#define MINILAB_KNOB_1_CC 7 // w/ Shift
#define MINILAB_KNOB_2_CC 74
#define MINILAB_KNOB_3_CC 71
#define MINILAB_KNOB_4_CC 76
#define MINILAB_KNOB_5_CC 77
#define MINILAB_KNOB_6_CC 93
#define MINILAB_KNOB_7_CC 73
#define MINILAB_KNOB_8_CC 75
#define MINILAB_KNOB_9_CC 116 // w/ Shift
#define MINILAB_KNOB_10_CC 18
#define MINILAB_KNOB_11_CC 19
#define MINILAB_KNOB_12_CC 16
#define MINILAB_KNOB_13_CC 17
#define MINILAB_KNOB_14_CC 91
#define MINILAB_KNOB_15_CC 79
#define MINILAB_KNOB_16_CC 72

#define MINILAB_PAD_1_NOTE 24 // NOTE_C1
#define MINILAB_PAD_2_NOTE 25 // NOTE_CS1
#define MINILAB_PAD_3_NOTE 26 // NOTE_D1
#define MINILAB_PAD_4_NOTE 27 // NOTE_DS1
#define MINILAB_PAD_5_NOTE 28 // NOTE_E1
#define MINILAB_PAD_6_NOTE 29 // NOTE_F1
#define MINILAB_PAD_7_NOTE 30 // NOTE_FS1
#define MINILAB_PAD_8_NOTE 31 // NOTE_G1

#define MINILAB_PAD_9_CC 22
#define MINILAB_PAD_10_CC 23
#define MINILAB_PAD_11_CC 24
#define MINILAB_PAD_12_CC 25
#define MINILAB_PAD_13_CC 26
#define MINILAB_PAD_14_CC 27
#define MINILAB_PAD_15_CC 28
#define MINILAB_PAD_16_CC 29


// Convert between 1-16 and SysEx IDs
byte padHardwareIdToNumber(byte padHardwareId);
byte padNumberToHardwareId(byte padNumber);

void setPadColor(byte padNumber, byte color);
void setButtonState(byte buttonHardwareId, byte color);

typedef struct minilabEvent
{
    byte button;
    bool isKeyDown;
} minilabEvent_t;

void handleMinilabEvent(minilabEvent_t e);
// Returns true if handled as a minilab event
bool handleAsMinilabEvent(midiEventPacket_t e);