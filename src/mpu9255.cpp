#include "mpu9255.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

namespace SENSOR {
	mpu9255::mpu9255() : ins_data(255, 0.0), ins_bias(255, 0.0)
	{
		if (!bcm2835_init()) {
			std::cout << "-- [ERROR]: Could not initialize raspberry pi low level hardware." << std::endl;
		}
	}

	mpu9255::~mpu9255() {}

	bool mpu9255::init()
	{
		bool rv = true;
		bcm2835_spi_end();
		if (!bcm2835_spi_begin())
		{
			rv = false;
			this->spi_init = false;
			printf("-- [ERROR]: BCM2835_spi_begin failed. Are you running as root??\n");
		}
		else {
			this->spi_init = true;
			rv = true;
			// Send MSB first
    		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);            
    		// Select mode 0: Clock Polarity = 0; Clock Phase = 0;
    		bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);
    		// Set the channel's speed to 12.5 MHz (RPi 3)      
    		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
    		// Set up CS pin
    		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    		// Set CS pin active state
    		bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
		}
		
		this->Axis = MPU9255_GYRO_X_ENABLE | MPU9255_GYRO_Y_ENABLE | MPU9255_GYRO_Z_ENABLE | \
			MPU9255_ACCEL_X_ENABLE | MPU9255_ACCEL_Y_ENABLE | MPU9255_ACCEL_Z_ENABLE;
		this->GyroConfig = MPU9255_FULLSCALE_250 | MPU9255_GYRO_FILTER_3;
		this->AccelConfig1 = MPU9255_FULLSCALE_2G;
		this->AccelConfig2 = MPU9255_ACCEL_FILTER_1 | MPU9255_BANDWIDTH_2;
		this->MagnetoConfig = 0x16;
		this->Config = 0;
		this->PowerMnt1 = 0x01;
		this->Bandwidth = MPU9255_BANDWIDTH_4;

		char reg_cmd = 0;
		if (this->write_reg(reg_cmd, (char) MPU9255_PWR_MGMT_1_ADDR, 1) == false) rv = false;

		/* Write to Power management 1 register */
		reg_cmd 		= this->PowerMnt1;
		if (this->write_reg(reg_cmd, (char) MPU9255_PWR_MGMT_1_ADDR, 1) == false) rv = false;

		/* Write to Power management 2 register */
		reg_cmd 		= ~this->Axis;
		if (this->write_reg(reg_cmd, (char) MPU9255_PWR_MGMT_2_ADDR, 1) == false) rv = false;

		/* Config gyro operation */
		reg_cmd 		= this->GyroConfig;
		if (this->write_reg(reg_cmd, (char) MPU9255_GYRO_CONFIG_ADDR, 1) == false) rv = false;

		reg_cmd 		= this->Config | this->Bandwidth;
		if (this->write_reg(reg_cmd, (char) MPU9255_CONFIG_ADDR, 1) == false) rv = false;

		reg_cmd 		= this->AccelConfig1;
		if (this->write_reg(reg_cmd, (char) MPU9255_ACCEL_CONFIG_ADDR, 1) == false) rv = false;

		reg_cmd 		= this->AccelConfig2;
		if (this->write_reg(reg_cmd, (char) MPU9255_ACCEL_CONFIG_2_ADDR, 1) == false) rv = false;

		reg_cmd 		= this->IntPinConfig;
		// Reset all sensors' digital signal path
		reg_cmd 		= 0x07;
		mpu9255_write(reg_cmd, MPU9255_SIGNAL_PATH_RST_ADDR, 1);

		// Reset all sensors' register and signal path
		reg_cmd 		= 0x01;
		mpu9255_write(reg_cmd, MPU9255_USER_CTRL_ADDR, 1);

		if (this->MagnetoConfig != 0x00)
		{
			// Reset I2C slave module
			reg_cmd 		= 0x10;
			mpu9255_write(reg_cmd, MPU9255_USER_CTRL_ADDR, 1);

			// Enable I2C master
			reg_cmd 	= 0x20;
			if (this->write_reg(reg_cmd, (char) MPU9255_USER_CTRL_ADDR, 1) == false) rv = false;

			// Set I2C channel speed to 400 kHz
			if (this->write_reg(0x0D, (char) MPU9255_I2C_MASTER_CTRL, 1) == false) rv = false;

			if (this->write_reg(0x0c, (char) 37, 1) == false) rv = false;
			if (this->write_reg(0x0b, (char) 38, 1) == false) rv = false;
			if (this->write_reg(0x01, (char) 99, 1) == false) rv = false;
			if (this->write_reg(0x81, (char) 39, 1) == false) rv = false;

			// Set the address of AK8963 to slave 0 address register
			reg_cmd 	= 0x0C | READWRITE_CMD;
			if (this->write_reg(reg_cmd, (char) 37, 1) == false) rv = false;
			if (this->write_reg(0x00, (char) 38, 1) == false) rv = false;

			// Enable I2C master module and read 1 byte
			if (this->write_reg(0x81, (char) 39, 1) == false) rv = false;

			// Read 1 byte retrieved from AK8963
			mpu9255_read(0x49, &reg_cmd, 1);
			if (reg_cmd != 0x48) rv = false;

			// Set AK8963 address to write mode
			if (this->write_reg(0x0c, (char) 37, 1) == false) rv = false;
			if (this->write_reg(0x0b, (char) 38, 1) == false) rv = false;
			if (this->write_reg(0x01, (char) 99, 1) == false) rv = false;
			if (this->write_reg(0x81, (char) 39, 1) == false) rv = false;

			if (this->write_reg(0x0a, (char) 38, 1) == false) rv = false;
			if (this->write_reg(this->MagnetoConfig, (char) 99, 1) == false) rv = false;
			if (this->write_reg(0x81, (char) 39, 1) == false) rv = false;

			if (this->write_reg((0x0c | READWRITE_CMD), (char) 37, 1) == false) rv = false;
			if (this->write_reg(0x03, (char) 38, 1) == false) rv = false;
			if (this->write_reg(0x87, (char) 39, 1) == false) rv = false;
		}s
		return rv;
	}

	bool mpu9255::write_reg(char reg_cmd, char REG_ADDR, int num_byte)
	{
		bool rv = true;
		char data[2] = {(REG_ADDR), reg_cmd};
		bcm2835_spi_transfern(data, 2);

		sleep(0.1);

		char response[2] = {(REG_ADDR | READWRITE_CMD), 0x00};
		bcm2835_spi_transfern(response, 2);

		if (response[1] != reg_cmd) 
		{
			rv = false;
		}	
		return rv;
	}

	void mpu9255::read_reg(char REG_ADDR, char * pData, int num_byte)
	{
		char data[num_byte + 1] = {0};
		data[0] = REG_ADDR | READWRITE_CMD;
		bcm2835_spi_transfern(data, num_byte + 1);
		memcpy(pData, &data[1], num_byte*sizeof(char));
	}

	char mpu9255::get_id()
	{
		char ID = 0x00;

		this->read_reg(MPU9255_WHO_AM_I_ADDR, &ID, 1);
		return ID;
	}
}
