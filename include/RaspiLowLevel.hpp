
#ifndef RASPI_LOW_LEVEL_HPP
#define RASPI_LOW_LEVEL_HPP

#include "ros/ros.h"
#include <bcm2835.h>
#include <stdint.h>
#include <iostream>
#include <vector>

namespace RASPI {
	class RaspiLowLevel
	{
	public:
		RaspiLowLevel();
		~RaspiLowLevel();
		bool init_spi();
		bool init_gpio(uint16_t pins);
	private:
		std::vector<uint16_t> buffer;
	};
}

#endif
