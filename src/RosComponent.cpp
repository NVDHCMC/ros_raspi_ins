

#include "RosComponent.hpp"


namespace RTOS {
	// ROS component constructor
	RosComponent::RosComponent() {
		this->topic_name = "chatter";
		this->crap_data = this->ins_node.advertise<std_msgs::String>(this->topic_name, buffer_length);
		this->ins_data = this->ins_node.advertise<ins_controller::Ins>("Ins_Raw", buffer_length);
		this->random_value = 0;
		this->buffer_length = 1000;
	}
	
	RosComponent::RosComponent(std::string tname) {
		this->topic_name = tname;
		this->crap_data = this->ins_node.advertise<std_msgs::String>(this->topic_name, buffer_length);
		this->ins_data = this->ins_node.advertise<ins_controller::Ins>("Ins_Raw", buffer_length);
		this->random_value = 0;
		this->buffer_length = 1000;
	}
	
	// ROS component destructor
	RosComponent::~RosComponent() {}
	
	void RosComponent::send_data() {
		this->ss.clear();
		this->ss << "loop number " << random_value << ".";
		this->msg.data = this->ss.str();

		this->ins_message.
		this->ins_message.gyro_x = 0;
		this->ins_message.gyro_y = 0;
		this->ins_message.gyro_z = 0;
		this->ins_message.accel_x = 0;
		this->ins_message.accel_y = 0;
		this->ins_message.accel_z = 0;
		this->ins_message.magnetic_x = 0;
		this->ins_message.magnetic_y = 0;
		this->ins_message.magnetic_z = 0;

		this->crap_data.publish(this->msg);
		this->ins_data.publish(this->ins_message)
		ros::spinOnce();
	}
}

