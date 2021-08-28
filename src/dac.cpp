#include "dac.h"

MCP4922 DAC;

void dacSweep()
{
  Serial.println();
  Serial.println(__FUNCTION__);
  for (int channel = 0; channel < DAC.channels(); channel++)
  {
    Serial.println(channel);
    for (uint16_t value = 0; value < DAC.maxValue(); value += 0xFF)
    {
      DAC.analogWrite(value, channel);
      Serial.print(value);
      Serial.print("\t");
      Serial.println(analogRead(A0));
      delay(10);
    }
  }
}

uint16_t dacValue = 0; // 12bit max
unsigned long lastDacUpdate = 0;
void dac0Sweep()
{
  // Update one step every 100 milliseconds, which should result in 2^4*100ms = 1.60 seconds per sweep
  unsigned long now = millis();
  if (now - lastDacUpdate > 100) {
    uint8_t channel = 0;
    DAC.analogWrite(dacValue, channel);
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

void DacInit()
{
  DAC.begin(4);
  DAC.setGain(2);
}