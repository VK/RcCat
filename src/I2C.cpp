#include "I2C.h"

// use ports usually used for hardware I2C
//#define SDA_PORT PORTC
//#define SDA_PIN 4
//#define SCL_PORT PORTC
//#define SCL_PIN 5

#define SDA_PORT PORTB
#define SDA_PIN 2
#define SCL_PORT PORTB
#define SCL_PIN 3

#define I2C_FASTMODE 1
//#define I2C_SLOWMODE 1
//#define I2C_NOINTERRUPT 1

//#define I2C_TIMEOUT 100


#include "SoftI2CMaster.h"


I2C i2c;




void I2C::beginTransmission(uint8_t address) {
	if (transmitting) {
		error = (i2c_rep_start((address<<1)|I2C_WRITE) ? 0 : 2);
	} else {
		error = (i2c_start((address<<1)|I2C_WRITE) ? 0 : 2);
	}
	// indicate that we are transmitting
	transmitting = 1;
}


size_t I2C::write(uint8_t data) {
    if (i2c_write(data)) {
      return 1;
    } else {
      if (error == 0) error = 3;
      return 0;
    }
  }


	uint8_t I2C::endTransmission(uint8_t sendStop)
   {
     uint8_t transError = error;
     if (sendStop) {
       i2c_stop();
       transmitting = 0;
     }
     error = 0;
     return transError;
   }



	 uint8_t I2C::requestFrom(uint8_t address, uint8_t quantity,
	 		      uint32_t iaddress, uint8_t isize, uint8_t sendStop) {
	     uint8_t localerror = 0;
	     if (isize > 0) {
	       // send internal address; this mode allows sending a repeated start to access
	       // some devices' internal registers. This function is executed by the hardware
	       // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)
	       beginTransmission(address);
	       // the maximum size of internal address is 3 bytes
	       if (isize > 3){
	 	isize = 3;
	       }
	       // write internal register address - most significant byte first
	       while (isize-- > 0)
	 	write((uint8_t)(iaddress >> (isize*8)));
	       endTransmission(false);
	     }
	     // clamp to buffer length
	     if(quantity > BUFFER_LENGTH){
	       quantity = BUFFER_LENGTH;
	     }
	     localerror = !i2c_rep_start((address<<1) | I2C_READ);
	     if (error == 0 && localerror) error = 2;
	     // perform blocking read into buffer
	     for (uint8_t cnt=0; cnt < quantity; cnt++)
	       rxBuffer[cnt] = i2c_read(cnt == quantity-1);
	     // set rx buffer iterator vars
	     rxBufferIndex = 0;
	     rxBufferLength = quantity;
	     if (sendStop) {
	       transmitting = 0;
	       i2c_stop();
	     }
	     return quantity;
	   }




		   uint8_t I2C::read() {
		     uint8_t value = -1;
		     if(rxBufferIndex < rxBufferLength){
		       value = rxBuffer[rxBufferIndex];
		       ++rxBufferIndex;
		     }
		     return value;
		   }



///internally used functions
void I2C::writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	beginTransmission(address);  // Initialize the Tx buffer
	write(subAddress);           // Put slave register address in Tx buffer
	write(data);                 // Put data in Tx buffer
	endTransmission(true);           // Send the Tx buffer
}

void I2C::writeByteLongAddr(uint8_t address, uint16_t subAddress, uint8_t data)
{
	beginTransmission(address);  // Initialize the Tx buffer
	write((subAddress >> 8) & 0xFF); //MSB of register address
	write(subAddress & 0xFF); //LSB of register address
	write(data);                 // Put data in Tx buffer
	endTransmission(true);           // Send the Tx buffer
}


void I2C::write2Bytes(uint8_t address, uint8_t subAddress, uint16_t data)
{
	beginTransmission(address);  // Initialize the Tx buffer
	write(subAddress);           // Put slave register address in Tx buffer
	write(data);                 // Put data in Tx buffer
	uint8_t temp;
  temp = (data >> 8) & 0xff;
  write(temp);
  temp = data & 0xff;
  write(temp);
	endTransmission(true);           // Send the Tx buffer
}

void I2C::write2BytesLongAddr(uint8_t address, uint16_t subAddress, uint16_t data)
{
	beginTransmission(address);  // Initialize the Tx buffer
	write((subAddress >> 8) & 0xFF); //MSB of register address
	write(subAddress & 0xFF); //LSB of register address
  uint8_t temp;
	temp = (data >> 8) & 0xff;
	write(temp);
	temp = data & 0xff;
	write(temp);
	endTransmission(true);           // Send the Tx buffer
}


uint8_t I2C::readByte(uint8_t address, uint8_t subAddress)
{
	uint8_t data;
	requestFrom(address, (size_t) 1,  subAddress, 1, true);
	data = read();
  return data;
}


uint8_t I2C::readByteLongAddr(uint8_t address, uint16_t subAddress)
{
	uint8_t data;
	requestFrom(address, (size_t) 1,  subAddress, 2, true);
	data = read();
  return data;
}


uint16_t I2C::read2Bytes(uint8_t address, uint8_t subAddress)
{
	uint8_t data_low;
	uint8_t data_high;
	uint16_t data;
	requestFrom(address, (size_t) 2,  subAddress, 1, true);
	data_high = read();
  data_low = read();
	data = (data_high << 8)|data_low;
  return data;
}


uint16_t I2C::read2BytesLongAddr(uint8_t address, uint16_t subAddress)
{
	uint8_t data_low;
	uint8_t data_high;
	uint16_t data;
	requestFrom(address, (size_t) 2,  subAddress, 2, true);
	data_high = read();
	data_low = read();
	data = (data_high << 8)|data_low;
  return data;
}


void I2C::readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{
  //Serial.println(
  requestFrom(address, (size_t) count, subAddress, 1, true);  // Read bytes from slave register address

	for (int i = 0; i < rxBufferLength ; i++)
	{
		dest[i] = rxBuffer[i];
	}


}



I2C::I2C()
{
	transmitting = false;
  error = 0;
	rxBufferIndex = 0;
  rxBufferLength = 0;
  i2c_init();

}
