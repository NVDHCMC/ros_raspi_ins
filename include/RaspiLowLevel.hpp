
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
		void command_stm32();
		std::vector<float> ins_data;
	private:
		bool init;
		bool spi_init;
		int stm32_init_string[10];
		int stm32_receive_string[10];
		int stm32_pair_string[10];
		int stm32_accept_string[10];
		int raw_spi_data[20];
		int dummy_string[20];
		std::vector<uint16_t> buffer;
	};

}

#endif
