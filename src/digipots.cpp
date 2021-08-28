#include "digipots.h"

const byte digipots[4] = {
    MCP4451_COMMAND(MCP4451_WIPER0, MCP4451_WRITE, 0b00),
    MCP4451_COMMAND(MCP4451_WIPER1, MCP4451_WRITE, 0b00),
    MCP4451_COMMAND(MCP4451_WIPER2, MCP4451_WRITE, 0b00),
    MCP4451_COMMAND(MCP4451_WIPER3, MCP4451_WRITE, 0b00)};

void i2c_send(byte addr, byte a, byte b) //wrapper for I2C routines
{
    // Serial.print("Sending i2c message ");
    // Serial.print(addr, 16);
    // Serial.print(" ");
    // Serial.print(a, 16);
    // Serial.print(" ");
    // Serial.print(b, 16);
    // Serial.println("");

#ifdef USE_MOZZI_TWI
    // source: https://github.com/Roger-random/Mozzi_MMA7660
    twowire_beginTransmission(addr);
    twowire_send(a);
    twowire_send(b);
    twowire_endTransmission();
#else
    Wire.beginTransmission(addr);
    Wire.write(a);
    Wire.write(b);
    Wire.endTransmission();
#endif
    // Serial.print("i2c result ");
    // Serial.println(result);
}

void DigipotInit()
{
#ifdef TWI_NONBLOCK_H_
    initialize_twi_nonblock();
#else
    Wire.begin();
#endif

    // enable all digipots
    i2c_send(MCP4451_I2C_ADDR, MCP4451_COMMAND(MCP4451_TCON0, MCP4451_WRITE, 0b00), 0xff);
    i2c_send(MCP4451_I2C_ADDR, MCP4451_COMMAND(MCP4451_TCON1, MCP4451_WRITE, 0b00), 0xff);
}

//write a value on one of the four digipots in the IC
void DigipotWrite(byte pot, byte val)
{
    // set specific digipot value
    i2c_send(MCP4451_I2C_ADDR, digipots[pot], val);
}

void DigipotTestSweep()
{
    Serial.println("Sweeping pots");
    for (int pot = 0; pot < 1; pot++)
    {
        Serial.print("pot ");
        Serial.println(pot);
        for (uint16_t value = 0; value < 0xff; value++)
        {
            DigipotWrite(pot, value);
            Serial.print(value);
            Serial.print("\t");
            Serial.println(analogRead(A1));
            delay(10);
        }
    }
}