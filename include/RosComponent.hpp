

#ifndef ROSCOMPONENT_HPP
#define ROSCOMPONENT_HPP

#include "ros/ros.h"
#include <std_msgs/String.h>
#include <iostream>
#include <string>
#include <sstream>
#include "ins_controller/Ins.h"

namespace RTOS {
	
	class RosComponent {
	public:
		RosComponent();
		RosComponent(std::string tname);
		~RosComponent();
		void send_data(std::vector<float> RPY);
	private:
		int random_value;
		int buffer_length;
	
		std::string topic_name;
	
		ros::NodeHandle ins_node;
		ros::Publisher crap_data;
		std_msgs::String msg;

		ins_controller::Ins ins_message;
		ros::Publisher ins_data;
	};

}

#endif
