

#include "ros/ros.h"
#include <std_msgs/String.h>
#include <iostream>
#include <string>
#include <sstream>


class RosComponent
{
public:
	RosComponent();
	~RosComponent();
	void send_data();
private:
	int random_value = 0;
	int buffer_length = 1000;

	std::string topic_name;
	std::stringstream ss;

	ros::NodeHandle ins_node;
	ros::Publisher ins_data;
	std_msgs::String msg;
};

// ROS component constructor
RosComponent::RosComponent() {
	this->topic_name = "chatter";
	this->ins_data = this->ins_node.advertise<std_msgs::String>(this->topic_name, buffer_length);
}

RosComponent::RosComponent(std::string tname) {
	this->topic_name = tname;
	this->ins_data = this->ins_node.advertise<std_msgs::String>(this->topic_name, buffer_length);
}

// ROS component destructor
RosComponent::~RosComponent() {}

void RosComponent::send_data() {
	this->ss << "loop number " << random_value << ".";
	this->msg.data = this->ss.str();
		
	this->ins_data.publish(msg);
		
	ros::spinOnce();
}