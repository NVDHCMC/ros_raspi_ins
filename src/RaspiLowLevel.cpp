#include "RaspiLowLevel.hpp"

namespace RASPI {
	RaspiLowLevel::RaspiLowLevel() : buffer(255, 0) {
		// Initialize bcm2835 low-level api
		if (!bcm2835_init()) {
			std::cout << "Could not initialize raspberry pi low level hardware." << std::endl;
		}
	}

	RaspiLowLevel::~RaspiLowLevel() {

	}

	bool RaspiLowLevel::init_spi() {
		bool rv = true;

		return rv;
	}

	bool RaspiLowLevel::init_gpio(uint16_t pins) {
		bool rv = true;

		return rv;
	} 
}
