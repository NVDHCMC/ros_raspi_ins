#include "RaspiLowLevel.hpp"

namespace RASPI {
	RaspiLowLevel::RaspiLowLevel() : buffer(255, 0), \
		stm32_init_string(0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), \
		stm32_receive_string(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), \
		raw_spi_data(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), \
		dummy_string(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00) {
		// Initialize bcm2835 low-level api
		if (!bcm2835_init()) {
			std::cout << "Could not initialize raspberry pi low level hardware." << std::endl;
			this->init = false;
		}
		else {
			this->init = true;
		}
	}

	RaspiLowLevel::~RaspiLowLevel() {

	}

	bool RaspiLowLevel::init_spi() {
		bool rv = true;
		if (!bcm2835_spi_begin())
		{
			this->spi_init = false;
			rv = false;
			printf("bcm2835_spi_begin failed. Are you running as root??\n");
		}
		else {
			this->spi_init = true;
			rv = true;
			// Send MSB first
    		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);            
    		// Select mode 0: Clock Polarity = 0; Clock Phase = 0;
    		bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    		// Set the channel's speed to 12.5 MHz (RPi 3)      
    		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048);
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
			std::cout << "The SPI module wasn't properly initialized." << std::endl;
		}
		else {
        	bcm2835_spi_transfern(this->stm32_init_string, 11);

        	uint8_t i = 0;

        	for(i = 0; i < 11; i++) {
        		if (this->stm32_init_string[i] != this->stm32_receive_string[i]) {
        			rv = false;
        			break;
        		}
        	}
        	rv = true;
		}
		return rv;
	}

	bool RaspiLowLevel::pair_stm32() {
		bool rv = true;

		return rv;
	}

	void RaspiLowLevel::fetch_data_from_stm32(std::vector<float> * data) {
		bcm2835_spi_transfern(this->dummy_string, 10);

		memcpy(this->raw_spi_data, this->dummy_string, 10);

		uint8_t i = 0;
		for ( i = 0; i < 10; i++) {
			this->dummy_string[i] = 0x00;
		}
	}
}
