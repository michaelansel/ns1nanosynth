// Pads: 112 - 127 (0x70-7F) (multicolor)
// Shift: 46 (0x2E)
// PadSelect: 47 (0x2F)
// Oct-: 16 (0x10)
// Oct+: 17 (0x11)

// Set color: F0 00 20 6B 7F 42 02 00 10 ID COLOR F7
// Key down: F0 00 20 6B 7F 42 02 00 00 ID 7F F7
// Key up:   F0 00 20 6B 7F 42 02 00 00 ID 7F F7

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