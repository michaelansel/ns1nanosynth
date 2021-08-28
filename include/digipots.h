#include "main.h"

#ifdef USE_MOZZI_TWI
#include "twi_nonblock.h"
#else
#include "Wire.h" // i2c for digipots
#endif

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

const char digipotNames[] = {'A','B','C','D'};

void DigipotInit();
void DigipotWrite(byte pot, byte val);
void DigipotTestSweep();