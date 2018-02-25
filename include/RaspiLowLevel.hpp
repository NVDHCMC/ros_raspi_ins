
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
		bool init_stm32();
		bool pair_stm32();
		void fetch_data_from_stm32(std::vector<float> * data);
	private:
		bool init;
		bool spi_init;
		char stm32_init_string[11];
		char stm32_receive_string[11];
		char raw_spi_data[10];
		char dummy_string[10];
		std::vector<uint16_t> buffer;
	};

}

#endif
