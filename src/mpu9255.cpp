#include <stdint>
#include "RaspiLowLevel.hpp"
#include "mpu9255.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

namespace SENSOR {
	mpu9255::mpu9255(): ins_data(255, 0.0), ins_bias(255, 0.0)
	{
		if (!bcm2835_spi_begin())
		{
			this->spi_init = false;
			printf("-- [ERROR]: BCM2835_spi_begin failed. Are you running as root??\n");
		}
		else {
			this->spi_init = true;
			// Send MSB first
    		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);            
    		// Select mode 0: Clock Polarity = 0; Clock Phase = 0;
    		bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    		// Set the channel's speed to 12.5 MHz (RPi 3)      
    		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
    		// Set up CS pin
    		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    		// Set CS pin active state
    		bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
		}
	}

	mpu9255::~mpu9255() {}

	bool mpu9255::write_reg(uint8_t reg_cmd, uint8_t REG_ADDR, int num_byte)
	{
		bool rv = true;
		uint8_t data[2] = {(REG_ADDR), reg_cmd};
		bcm2835_spi_transfern(data, 2);

		sleep(0.1);

		uint8_t response[2] = {(REG_ADDR | READWRITE_CMD), 0x00};
		bcm2835_spi_transfern(data, 2);

		if (response[1] != reg_cmd) 
		{
			rv = false;
		}	
		return rv;
	}

	void mpu9255::read_reg(uint8_t REG_ADDR, uint8_t * pData, int num_byte)
	{
		uint8_t data[num_byte + 1] = {0};
		data[0] = REG_ADDR | READWRITE_CMD;
		bcm2835_spi_transfern(data, num_byte + 1);
		memcpy(pData, &data[1], num_byte);
	}

	uint8_t mpu9255::get_id()
	{
		uint8_t ID = 0x00;

		this->read_reg(MPU9255_WHO_AM_I_ADDR, &ID, 1);
		return ID;
	}
}
