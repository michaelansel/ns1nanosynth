#include "main.h"

#include <MozziGuts.h> // Load first so digipots don't use Wire
void MozziInit();
#include "digipots.h"
#include "dac.h"
#include "midi.h"
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
  midiHook(); // midi processing
  audioHook(); // mozzi processing
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

void MozziInit() {
  startMozzi(CONTROL_RATE);
  aSin.setFreq(440);
  int attack = 30;
  int decay = 500;
  kEnvelope.set(attack,decay);
}

void updateControl(){
  int envgain = (int) kEnvelope.next();
  if ( envgain < 5 ){
    // Looping envelope
    kEnvelope.start();
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
  if (e.header == MIDI_CC) {
    switch(e.byte2) {
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