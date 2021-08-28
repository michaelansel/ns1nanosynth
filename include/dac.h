#include "main.h"

#include <SPI.h> // spi for DAC
#include "MCP_DAC.h"
extern MCP4922 DAC;  // HW SPI


void DacInit();
void dac0Sweep();
void dacSweep();