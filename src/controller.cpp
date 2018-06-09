#include "controller.hpp"

namespace RASPI {
	controller::controller(uint8_t addr) : current_pos(3, 0.0), target_pos(3, 0.0), control_params(3, 0.0), motor_params(3, 0.0), current_state(3, 0.0)
	{
		this->Ky = 0.00064;
		this->Ko = 0.016;
		this->W = 220;
		this->L = 337;
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
		int i = 0;
		while (bcm2835_i2c_read(this->receive, 32) != BCM2835_I2C_REASON_OK)
		{
			i++;
			if (i > 5)
			{
				printf("Receive error\n");
				break;
			}
		}
	}

	void controller::test_send(char * mess, uint8_t len)
	{
		int i = 0;
		while (bcm2835_i2c_write(mess, len) != BCM2835_I2C_REASON_OK)
		{
			i++;
			if (i > 5)
			{
				printf("Send error\n");
				break;
			}
		}
	}

	void controller::kanamaya(float e2, float e3)
	{
		// Constant speed vR = 300 mm/s
		this->control_params.at(0) = 300*cos(e3);
		this->control_params.at(1) = 300*(this->Ky*e2 + this->Ko*sin(e3));
	}

	void controller::calculate_speed_for_motors()
	{
		if (this->control_params.at(1) == 0.0f)
		{
			this->motor_params.at(0) = round(this->control_params.at(0)*0.30123614f);
			this->motor_params.at(1) = this->motor_params.at(0);
			this->motor_params.at(2) = 0.0f;
		}
		else
		{
			float R = this->control_params.at(0)/this->control_params.at(1);
			this->motor_params.at(0) = round((R - this->W/2.0f)*this->control_params.at(1)*0.30123614f);
			this->motor_params.at(1) = round((R + this->W/2.0f)*this->control_params.at(1)*0.30123614f);
			this->motor_params.at(2) = atan2(this->L, R)*57.2957795f;
		}
	}

	void controller::serialization()
	{
		int vl = (int) this->motor_params.at(1);
		int vr = (int) this->motor_params.at(0);
		int angle = (int) this->motor_params.at(2)*100;
		this->send[0] = 0xff;
		this->send[1] = 0x03;
		this->send[2] = 'l';
		this->send[3] = (vl >> 8) & 0xff;
		this->send[4] = (vl & 0xff);
		this->send[5] = 'r';
		this->send[6] = (vr >> 8) & 0xff;
		this->send[7] = (vr & 0xff);
		this->send[8] = 'd';
		this->send[9] = angle/1000;
		this->send[10] = (angle % 1000) / 100;
		this->send[11] = (angle % 100) / 10;
		this->send[12] = (angle % 10);
		this->send[13] = '\r';
	}

	void controller::get_pos()
	{
		this->test_receive();
		this->current_state.at(0) = this->receive[2]*256 + this->receive[3];
		this->current_state.at(1) = this->receive[0]*256 + this->receive[1];
		if (this->current_state.at(0) > 32768)
			this->current_state.at(0) -= 65536;
		if (this->current_state.at(1) > 32768)
			this->current_state.at(1) -= 65536;
	}

	void controller::estimate_pos()
	{
		this->current_pos.at(0) += (this->current_state.at(0) + this->current_state.at(1))*0.005f/0.30123614f;
	}
}
