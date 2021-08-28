#define MOZZI 1

#include <Arduino.h>
#include "MIDIUSB.h"

#if MOZZI
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <Ead.h> // exponential attack decay
#include <tables/sin2048_int8.h> // sine table for oscillator

#undef CONTROL_RATE
#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;
#endif

#if MOZZI
#include "twi_nonblock.h"
#else
#include "Wire.h" // i2c for digipots
#endif

#include <SPI.h> // spi for DAC
#include "MCP_DAC.h"
MCP4922 DAC;  // HW SPI

// const byte NOTEON = 0x09;
// const byte NOTEOFF = 0x08;
const byte CC = 0x0B;
// const byte PB = 0x0E;


// Main i2c address
#define MCP4451_I2C_ADDR 0b0101100 // 0b01011 + A0(0) + A1(0)

// 4-bit memory addresses
#define MCP4451_TCON0 0x4
#define MCP4451_TCON1 0xa
#define MCP4451_WIPER0 0x0
#define MCP4451_WIPER1 0x1
#define MCP4451_WIPER2 0x6
#define MCP4451_WIPER3 0x7

// 2 bit commands
#define MCP4451_WRITE 0b00
#define MCP4451_INCREMENT 0b01
#define MCP4451_DECREMENT 0b10
#define MCP4451_READ 0b11

// Command byte = 4bit address + 2bit command + 2bit (MSB) of 10bit data (0x00)
#define MCP4451_COMMAND(address, command, data) (address << 4 | command << 2 | data)

byte digipots[4] = {
  MCP4451_COMMAND(MCP4451_WIPER0, MCP4451_WRITE, 0b00),
  MCP4451_COMMAND(MCP4451_WIPER1, MCP4451_WRITE, 0b00),
  MCP4451_COMMAND(MCP4451_WIPER2, MCP4451_WRITE, 0b00),
  MCP4451_COMMAND(MCP4451_WIPER3, MCP4451_WRITE, 0b00)
};

void i2c_send(byte addr, byte a, byte b)      //wrapper for I2C routines
{
  // Serial.print("Sending i2c message ");
  // Serial.print(addr, 16);
  // Serial.print(" ");
  // Serial.print(a, 16);
  // Serial.print(" ");
  // Serial.print(b, 16);
  // Serial.println("");

#ifdef TWI_NONBLOCK_H_
  // source: https://github.com/Roger-random/Mozzi_MMA7660
  twowire_beginTransmission(addr);
  twowire_send(a);
  twowire_send(b);
  byte result = twowire_endTransmission();
#else
  Wire.beginTransmission(addr);
  Wire.write(a);
  Wire.write(b);
  byte result = Wire.endTransmission();
#endif
  // Serial.print("i2c result ");
  // Serial.println(result);
}

//write a value on one of the four digipots in the IC
void DigipotWrite(byte pot,byte val)
{
  // set specific digipot value
  i2c_send( MCP4451_I2C_ADDR, digipots[pot], val);  
}

void setup()
{
  Serial.begin(9600);
  Serial.println(__FILE__);

#ifdef TWI_NONBLOCK_H_
  initialize_twi_nonblock();
#else
  Wire.begin();
#endif

  // enable all digipots 
  i2c_send( MCP4451_I2C_ADDR, MCP4451_COMMAND(MCP4451_TCON0, MCP4451_WRITE, 0b00), 0xff );
  i2c_send( MCP4451_I2C_ADDR, MCP4451_COMMAND(MCP4451_TCON1, MCP4451_WRITE, 0b00), 0xff );

  DAC.begin(4);
  DAC.setGain(2);

#if MOZZI
  startMozzi(CONTROL_RATE);
  aSin.setFreq(440);
  int attack = 30;
  int decay = 500;
  kEnvelope.set(attack,decay);
#endif

  Serial.println("\nSetup Done");
}


// void dacSweep()
// {
//   Serial.println();
//   Serial.println(__FUNCTION__);
//   for (int channel = 0; channel < DAC.channels(); channel++)
//   {
//     Serial.println(channel);
//     for (uint16_t value = 0; value < DAC.maxValue(); value += 0xFF)
//     {
//       DAC.analogWrite(value, channel);
//       Serial.print(value);
//       Serial.print("\t");
//       Serial.println(analogRead(A0));
//       delay(10);
//     }
//   }
// }

// void digipotSweep() {
//   Serial.println("Sweeping pots");
//   for (int pot = 0; pot < 1; pot++)
//   {
//     Serial.print("pot ");
//     Serial.println(pot);
//     for (uint16_t value = 0; value < 0xff; value++)
//     {
//       DigipotWrite(pot, value);
//       Serial.print(value);
//       Serial.print("\t");
//       Serial.println(analogRead(A1));
//       delay(10);
//     }
//   }
// }

uint16_t dacValue = 0; // 12bit max
unsigned long lastDacUpdate = 0;
void dac0Sweep()
{
  // Update one step every 100 milliseconds, which should result in 2^4*100ms = 1.60 seconds per sweep
  unsigned long now = millis();
  if (now - lastDacUpdate > 100) {
    uint8_t channel = 0;
    bool result = DAC.analogWrite(dacValue, channel);
    dacValue += 0xFF; // 256 steps in 16bit integer; 16 steps in 12-bit
    if (dacValue > DAC.maxValue()) {
      dacValue = 0;
      // Serial.print("MCP_DAC_LIB_VERSION: ");
      // Serial.println(MCP_DAC_LIB_VERSION);
      // Serial.println();
      // Serial.print("CHANNELS:\t");
      // Serial.println(DAC.channels());
      // Serial.print("MAXVALUE:\t");
      // Serial.println(DAC.maxValue());
    }
    lastDacUpdate = now;
    // Serial.print("dac ");
    // Serial.print(dacValue);
    // Serial.print(" (");
    // Serial.print(result ? "true" : "false");
    // Serial.println(")");
  }
}

#if MOZZI
void updateControl(){
  int envgain = (int) kEnvelope.next();
  if ( envgain < 5 ){
    kEnvelope.start();
  }

  gain = envgain;
}


int updateAudio(){
  return (aSin.next()*gain)>>8;
}
#endif

const char digipotNames[] = {'A','B','C','D'};

void loop(){
  // digipotSweep();
  // dacSweep();
  dac0Sweep();

  midiEventPacket_t e;
  e = MidiUSB.read();
  // Serial.println("new midi message received");

  // CC30-33 -> Digipot A-D
  if (e.header == CC && e.byte2 >= 30 && e.byte2 <= 33) {
    char potId = e.byte2 - 30;

    // Serial.print("pot");
    // Serial.print(digipotNames[potId]);
    // Serial.print("\t");
    // Serial.print(e.byte3);
    // Serial.print("\t");
    // Serial.println(e.byte3 << 1);

    DigipotWrite(potId, e.byte3<<1);
  }

  // CC35 -> Pin 5 (binary)
  if (e.header == CC && e.byte2 == 35) {
    if (e.byte3 >= 64) {
      digitalWrite(5, HIGH);
    } else {
      digitalWrite(5, LOW);
    }
  }

  // CC34 -> DAC1
  if (e.header == CC && e.byte2 == 34) {
    DAC.analogWrite(e.byte3 << 5, 1);
  }

  //set digipots A to D with CC from 30 to 33
  // if (e.header == CC && e.byte2 == 30){
  //   Serial.print("potA ");
  //   Serial.println(e.byte3);
  //   DigipotWrite(0,e.byte3<<1);
  // }
  // if (e.header == CC && e.byte2 == 31){
  //   Serial.print("potB ");
  //   Serial.println(e.byte3);
  //   DigipotWrite(1,e.byte3<<1);
  // }
  // if (e.header == CC && e.byte2 == 32){
  //   Serial.print("potC ");
  //   Serial.println(e.byte3);
  //   DigipotWrite(2,e.byte3<<1);
  // }
  // if (e.header == CC && e.byte2 == 33){
  //   Serial.print("potD ");
  //   Serial.println(e.byte3);
  //   DigipotWrite(3,e.byte3<<1);
  // }

#if MOZZI
  audioHook(); // required here
#endif
}