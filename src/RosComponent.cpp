

#include "RosComponent.hpp"


namespace {
	// ROS component constructor
	RosComponent::RosComponent() {
		this->topic_name = "chatter";
		this->ins_data = this->ins_node.advertise<std_msgs::String>(this->topic_name, buffer_length);
		this->random_value = 0;
		this->buffer_length = 1000;
	}
	
	RosComponent::RosComponent(std::string tname) {
		this->topic_name = tname;
		this->ins_data = this->ins_node.advertise<std_msgs::String>(this->topic_name, buffer_length);
		this->random_value = 0;
		this->buffer_length = 1000;
	}
	
	// ROS component destructor
	RosComponent::~RosComponent() {}
	
	void RosComponent::send_data() {
		this->ss << "loop number " << random_value << ".";
		this->msg.data = this->ss.str();
			
		this->ins_data.publish(msg);
			
		ros::spinOnce();
	}
}

