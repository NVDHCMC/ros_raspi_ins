#include "RaspiLowLevel.hpp"

namespace RASPI {
	RaspiLowLevel::RaspiLowLevel() : buffer(255, 0), ins_data(9, 0),
		stm32_init_string{0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		stm32_receive_string{0xff, 'H', 'e', 'l', 'l', 'o', ',', 'R', 'P', 'I', 0x00},
		stm32_pair_string{0xff, 'S', 'T', 'M', '3', '2', 'F', '4', '0', '7', 0x00},
		stm32_accept_string{0x43, 0x85, 0x31, 0x77, 0x8c, 0xc4, 0x8e, 0xc9, 0x4b, 0x11, 0x00},
		raw_spi_data{0x00},
		dummy_string{0x00} {
		// Initialize bcm2835 low-level api
		if (!bcm2835_init()) {
			std::cout << "-- [ERROR]: Could not initialize raspberry pi low level hardware." << std::endl;
			this->init = false;
		}
		else {
			this->init = true;
		}
	}

	RaspiLowLevel::~RaspiLowLevel() {
		bcm2835_spi_end();
	}

	bool RaspiLowLevel::init_spi() {
		bool rv = true;
		if (!bcm2835_spi_begin())
		{
			this->spi_init = false;
			rv = false;
			printf("-- [ERROR]: BCM2835_spi_begin failed. Are you running as root??\n");
		}
		else {
			this->spi_init = true;
			rv = true;
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
		return rv;
	}

	bool RaspiLowLevel::init_gpio(uint16_t pins) {
		bool rv = true;

		return rv;
	}

	bool RaspiLowLevel::init_stm32() {
		bool rv = true;
		if (!this->spi_init) {
			std::cout << "-- [ERROR]: The SPI module was not initialized." << std::endl;
			rv = false;
		}
		else {
			char * temp_str[11];
			memcpy(temp_str, this->stm32_init_string, 11);
        	
        	bcm2835_spi_transfern(temp_str, 11);

        	uint8_t i = 0;

        	for(i = 0; i < 11; i++) {
        		if (this->stm32_init_string[i] != this->stm32_receive_string[i]) {
        			rv = false;
        			break;
        		}
        	}

        	if (rv == true) 
        	{
        		printf("-- [INFO]: Complete STM32 initialization. \n");
        	}
		}
		return rv;
	}

	bool RaspiLowLevel::pair_stm32() {
		bool rv = true;
		uint8_t i = 0;
		if (this->init_stm32())
		{
			char * temp_str[11];

			memcpy(temp_str, this->stm32_pair_string, 11);
			sleep(0.5);
			bcm2835_spi_transfern(temp_str, 11);

			memcpy(temp_str, this->dummy_string, 11);
			sleep(0.5);
			bcm2835_spi_transfern(temp_str, 11);

			for(i = 0; i < 11; i++) {
        		if (this->stm32_accept_string[i] != temp_str) {
        			rv = false;
        			break;
        		}
        	}

			if (rv == true) {
				printf("-- [INFO]: Paired with STM32.\n");
			}

		}
		return rv;
	}

	void RaspiLowLevel::fetch_data_from_stm32(std::vector<float> * data) {

		char * temp_str[21];
		memcpy(temp_str, this->dummy_string, 21);
		bcm2835_spi_transfern(temp_str, 21);

		memcpy(this->raw_spi_data, temp_str, 21);

		uint8_t i = 0;
		for ( i = 0; i < 9; i++ ) {
			data->at(i) = (float) (((uint16_t) raw_spi_data[2*i]) << 8) + ((uint16_t) raw_spi_data[2*i + 1]);
			printf("%f ", data->at(i));
		}
		printf("\n");
	}

	void RaspiLowLevel::command_stm32() {

	}
}