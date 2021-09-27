#include "main.h"

#include <MozziGuts.h> // Load first so digipots don't use Wire
void MozziInit();
#include "digipots.h"
#include "dac.h"
#include "midi.h"
#include "arturia_minilabmk2.h"
#include "quantizer.h"

/* 
 * Boilerplate connecting code
 */

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  DigipotInit();
  DacInit();
  MozziInit();
  QuantizerInit();

  Serial.println("\nSetup Done");
}

void loop(){
  audioHook(); // mozzi processing
  // put everything else in updateControl to ensure high quality audio
}



/* 
 * The good stuff
 */

#include <Oscil.h> // oscillator template
#include <Ead.h> // exponential attack decay
#include <tables/sin2048_int8.h> // sine table for oscillator

#undef CONTROL_RATE
#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE
int gain;
bool envelopeEnabled = true;

void MozziInit() {
  startMozzi(CONTROL_RATE);
  aSin.setFreq(440);
  int attack = 30;
  int decay = 500;
  kEnvelope.set(attack,decay);
}

bool padOn = false;

void updateControl(){
  midiHook(); // midi processing

  int envgain = (int) kEnvelope.next();
  if ( envgain < 5 ){
    // Looping envelope
    kEnvelope.start();

    if (padOn) {
      setPadColor(1, MINILAB_GREEN);
      setPadColor(3, MINILAB_CYAN);
      padOn=false;
    } else {
      setPadColor(1, MINILAB_BLACK);
      setPadColor(3, MINILAB_BLACK);
      padOn=true;
    }
  }
  gain = envgain;

  // Quantize 1V/oct ADC1 -> DAC0
  DAC.analogWrite(quantizeAdcToDac(mozziAnalogRead(A1)), 0);
}

// Update M-> (pins 9,10)
int updateAudio(){
  return (aSin.next()*gain)>>8;
}

void handleMidiEvent(midiEventPacket_t e) {
  if(handleAsMinilabEvent(e)) return;

  if (e.header == MIDI_CC) {
    switch(e.byte2) {
      // CC25-28 -> Enveloped VCO
      case 25:
        // Attack
        // Map 0-127 -> 0-2048
        kEnvelope.setAttack(e.byte3 << 4);
        break;
      case 26:
        // Decay
        // Map 0-127 -> 0-2048
        kEnvelope.setDecay(e.byte3 << 4);
        break;
      case 27:
        // Enable/disable looping envelope
        // Map 0-127 -> 0-1
        envelopeEnabled = (e.byte3 >> 6) == 1;
        break;
      case 28:
        // Frequency
        // TODO maybe change to remove division?
        // Map 0-127 -> A1-A5, same as 1V/Oct for 0-5V
        aSin.setFreq((int)map(e.byte3, 0, 127, 55, 880));
        break;

      // CC30-33 -> Digipot A-D
      case 30 ... 33:
        // Map to pot id 0-3, and map value 0-127 -> 0-255
        DigipotWrite(e.byte2 - 30, e.byte3<<1);
        break;

      // CC34 -> DAC1
      case 34:
        // Map 0-127 -> 0-4095
        DAC.analogWrite(e.byte3 << 5, 1);
        break;

      // CC35-38 -> Pin 5-8 (binary)
      case 35 ... 38:
        // Map 0-127 -> 0-1
        digitalWrite(e.byte2 - 30, e.byte3 >> 6);
        break;
      
      default:
        break;
    }
  }
}


void handleMinilabEvent(minilabEvent_t e)
{
  // Serial.print("minilab button ");
  // Serial.print(e.button);
  // Serial.print(" ");
  // Serial.println(e.isKeyDown ? "down" : "up");
  switch(e.button) {
    case MINILAB_BUTTON_SHIFT:
    case MINILAB_BUTTON_PAD_SELECT:
    case MINILAB_BUTTON_OCTAVE_DOWN:
    case MINILAB_BUTTON_OCTAVE_UP:
    default:
      break;
  }
}