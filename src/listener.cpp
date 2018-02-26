#include "ros/ros.h"
#include "std_msgs/String.h"
#include "ins_controller/Ins.h"

void chatterCallback(const std_msgs::String::ConstPtr& msg) {
	ROS_INFO("I heard: [%s]", msg->data.c_str());
}

void insCallback(const ins_controller::Ins::ConstPtr& msg) {
	ROS_INFO("%f %f %f %f %f %f %f", msg->gyro_x, msg->gyro_y, msg->gyro_z, msg->accel_x,
		msg->accel_y, msg->accel_z, msg->magnetic_x);
}

int main(int argc, char ** argv) {
	ros::init(argc, argv, "listener");
	
	ros::NodeHandle n;
	
	ros::Subscriber sub = n.subscribe("chatter", 1000, chatterCallback);
	
	ros::Subscriber su2 = n.subscribe("Ins_Raw", 1000, insCallback);
	ros::spin();
	return 0;
}
