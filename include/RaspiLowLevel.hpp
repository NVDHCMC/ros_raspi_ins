
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
		void fetch_data_from_stm32(std::vector<float> * data, bool calibrated);
		void command_stm32();
		void calibrate();
		std::vector<float> ins_data;
		std::vector<float> ins_bias;
	private:
		bool init;
		bool spi_init;
		uint8_t stm32_init_string[11];
		uint8_t stm32_receive_string[11];
		uint8_t stm32_pair_string[11];
		uint8_t stm32_accept_string[11];
		uint8_t raw_spi_data[21];
		uint8_t dummy_string[21];
		uint32_t sample_count;
		std::vector<uint16_t> buffer;
	};

}

#endif
