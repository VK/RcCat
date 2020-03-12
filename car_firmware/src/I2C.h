#ifndef RCAT_I2C_H
#define RCAT_I2C_H

#include <Arduino.h>

#define BUFFER_LENGTH 32

class I2C
{
private:
  uint8_t transmitting;
  uint8_t error;
  uint8_t rxBuffer[BUFFER_LENGTH];
  uint8_t rxBufferIndex;
  uint8_t rxBufferLength;

public:
  I2C();
  void writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
  void writeByteLongAddr(uint8_t address, uint16_t subAddress, uint8_t data);
  uint8_t readByte(uint8_t address, uint8_t subAddress);
  uint8_t readByteLongAddr(uint8_t address, uint16_t subAddress);

  void write2Bytes(uint8_t address, uint8_t subAddress, uint16_t data);
  void write2BytesLongAddr(uint8_t address, uint16_t subAddress, uint16_t data);
  uint16_t read2Bytes(uint8_t address, uint8_t subAddress);
  uint16_t read2BytesLongAddr(uint8_t address, uint16_t subAddress);

  void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest);
  void beginTransmission(uint8_t address);
  size_t write(uint8_t data);
  uint8_t endTransmission(uint8_t sendStop);
  uint8_t requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop);
  uint8_t read();


};
extern I2C i2c;
#endif
