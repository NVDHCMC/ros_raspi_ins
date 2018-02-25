#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"

#include <sstream>

int ResultIncValue = 0;
int init_argc;
#define PERIOD_MICROSECS 10000 //10 millisecs
char ** init_argv;

class ROS_COMPONENT {
	public: 
		int a;
		ros::NodeHandle ins_node;
		ros::Publisher ins_data;
		void init();
		void send();
};

void ROS_COMPONENT::init() {
	this->ins_data = this->ins_node.advertise<std_msgs::String>("chatter", 1000);
}

void ROS_COMPONENT::send() {
	std_msgs::String msg;
		
	std::stringstream ss;
	ss << "loop number " << ResultIncValue << ".";
	msg.data = ss.str();
		
	this->ins_data.publish(msg);
		
	ros::spinOnce();
}

typedef boost::shared_ptr<ROS_COMPONENT> ROS_COMPONENT_ptr;
boost::shared_ptr<ROS_COMPONENT> test_ptr;

void * MySimpleTask( void * dummy )
{
	int i = 0;
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		test_ptr->send();
		ResultIncValue++;
	}
	return 0;
}

int main(int argc, char ** argv) {
	// Initialize ros with input params
	init_argc = argc;
	init_argv = argv;
	ros::init(argc, argv, "talker");
	// New node handler
	//ROS_COMPONENT_ptr new_ptr(new ROS_COMPONENT());
	test_ptr.reset(new ROS_COMPONENT());
	test_ptr->init();
	/**
	 * Create an advertiser to publish on a topic.
	 */
	
	int err;
	
	err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 16/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
	
	if ( err!=0 )
	{
		printf( "Init task error (%d)!\n",err );
	}
	else
	{
		printf( "Wait 1 seconds before ending...\n" );
		sleep( 10 );
		RTOS::ThreadRunning = 0;
		printf( "Increment value during 10 secs = %d (should be %d)\n", ResultIncValue, (9*1000*1000)/PERIOD_MICROSECS );
	}
	
	return 0;
	
}
