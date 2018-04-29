
#ifndef RASPI_LOW_LEVEL_HPP
#define RASPI_LOW_LEVEL_HPP

#include "ros/ros.h"
#include <bcm2835.h>
#include <stdint.h>
#include <iostream>
#include <vector>

namespace RASPI {
	class controller
	{
	public:
		controller(uint8_t addr);
		~controller();
		bool init_i2c();
		void get_pos();
		void calculate_speed(float current_angle_ref, float current_speed_ref);

		void test_send(char * mess, uint8_t len);
		void test_receive();
		float target_angle_ref;
		std::vector<float> current_pos;
		char receive[32];
		char send[32];
	private:
		bool i2c_init;
		std::vector<float> target_pos;
		uint8_t slave_addr;
	};
}

#endif
