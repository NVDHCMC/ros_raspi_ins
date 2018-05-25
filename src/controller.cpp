#include "controller.hpp"

namespace RASPI {
	controller::controller(uint8_t addr) : current_pos(3, 0.0), target_pos(3, 0.0) 
	{
		if (!bcm2835_init()) {
			std::cout << "-- [ERROR]: Could not initialize raspberry pi low level hardware." << std::endl;
		}
		memset(this->receive, 0x00, 32);
		this->slave_addr = addr;
	}

	controller::~controller()
	{

	}

	bool controller::init_i2c() 
	{
		bool rv = true;
		bcm2835_i2c_end();
		if (!bcm2835_i2c_begin())
		{
			rv = false;
			this->i2c_init = false;
			printf("-- [ERROR]: BCM2835_spi_begin failed. Are you running as root??\n");
		}
		else {
			this->i2c_init = true;
			rv = true;
			// Set slave address
			bcm2835_i2c_setSlaveAddress(this->slave_addr);
			// Set channel speed 400kHz
			bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
		}

		return rv;
	}

	void controller::test_receive()
	{
		bcm2835_i2c_read(this->receive, 32);
	}

	void controller::test_send(char * mess, uint8_t len)
	{
		bcm2835_i2c_write(mess, len);
	}
}