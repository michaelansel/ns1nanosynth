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
Oscil <SIN2048_NUM_CELLS, CONTROL_RATE> lfo1(SIN2048_DATA);
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
    if (envelopeEnabled) kEnvelope.start();

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

  // Map [-128:127] -> [0:4095]
  DAC.analogWrite((lfo1.next()+128)<<4, 1);
}

// Update M-> (pins 9,10)
int updateAudio(){
  return (aSin.next()*gain)>>8;
}

void handleMidiEvent(midiEventPacket_t e) {
  if(handleAsMinilabEvent(e)) return;

  if (e.header == MIDI_CC) {
    // Display via LED
    analogWrite(13, e.byte3 << 1);

    switch(e.byte2) {
      // Enveloped VCO
      case MINILAB_KNOB_2_CC:
        // Attack
        // Map 0-127 -> 0-2048
        kEnvelope.setAttack(e.byte3 << 4);
        break;
      case MINILAB_KNOB_3_CC:
        // Decay
        // Map 0-127 -> 0-2048
        kEnvelope.setDecay(e.byte3 << 4);
        break;
      case MINILAB_KNOB_10_CC:
        // Enable/disable looping envelope
        // Map 0-127 -> 0-1
        envelopeEnabled = (e.byte3 >> 6) == 1;
        break;
      case MINILAB_KNOB_11_CC:
        // Frequency
        // TODO maybe change to remove division?
        // Map 0-127 -> A1-A5, same as 1V/Oct for 0-5V
        aSin.setFreq((int)map(e.byte3, 0, 127, 55, 880));
        break;

      // Digipot A-D
      case MINILAB_KNOB_4_CC:
        // Map value 0-127 -> 0-255
        DigipotWrite(0, e.byte3<<1);
        break;
      case MINILAB_KNOB_5_CC:
        // Map value 0-127 -> 0-255
        DigipotWrite(1, e.byte3<<1);
        break;
      case MINILAB_KNOB_12_CC:
        // Map value 0-127 -> 0-255
        DigipotWrite(2, e.byte3<<1);
        break;
      case MINILAB_KNOB_13_CC:
        // Map value 0-127 -> 0-255
        DigipotWrite(3, e.byte3<<1);
        break;

      // LFO
      case MINILAB_KNOB_6_CC:
        // Frequency
        // Map 0-127 -> 1/100 - 16Hz
        float frequency = ((float)map(e.byte3, 0, 127, 1, (/*max nyquist*/CONTROL_RATE/4)*(/*divisor*/SIN2048_NUM_CELLS/CONTROL_RATE))) / (/*1 step per control update*/SIN2048_NUM_CELLS/CONTROL_RATE);
        Serial.println(frequency);
        lfo1.setFreq(frequency);
        break;
      case MINILAB_KNOB_14_CC:
        // Phase
        // Map 0-127 -> 0-SIN2048_NUM_CELLS
        lfo1.setPhase(map(e.byte3, 0, 127, 0, SIN2048_NUM_CELLS));

      // Undefined
      case MINILAB_KNOB_7_CC:
      case MINILAB_KNOB_8_CC:
      case MINILAB_KNOB_15_CC:
      case MINILAB_KNOB_16_CC:
        break;

      // // CC35-38 -> Pin 5-8 (binary)
      // case 35 ... 38:
      //   // Map 0-127 -> 0-1
      //   digitalWrite(e.byte2 - 30, e.byte3 >> 6);
      //   break;
      
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