

#ifndef ROSCOMPONENT_HPP
#define ROSCOMPONENT_HPP

#include "ros/ros.h"
#include <std_msgs/String.h>
#include <iostream>
#include <string>
#include <sstream>

namespace RTOS {
	
	class RosComponent {
	public:
		RosComponent();
		RosComponent(std::string tname);
		~RosComponent();
		void send_data();
	private:
		int random_value;
		int buffer_length;
	
		std::string topic_name;
		std::stringstream ss;
	
		ros::NodeHandle ins_node;
		ros::Publisher ins_data;
		std_msgs::String msg;
	};

}

#endif
