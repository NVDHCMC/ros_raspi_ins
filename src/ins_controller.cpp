#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"
#include "RosComponent.hpp"
#include "RaspiLowLevel.hpp"
#include <sstream>

int ResultIncValue = 0;
#define PERIOD_MICROSECS 10000 //10 millisecs

typedef boost::shared_ptr<RTOS::RosComponent> pRosComponent;
boost::shared_ptr<RTOS::RosComponent> pRosComp;

void * MySimpleTask( void * dummy )
{
	int i = 0;
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		pRosComp->send();
		ResultIncValue++;
	}
	return 0;
}

int main(int argc, char ** argv) {
	// Initialize ros with input params
	ros::init(argc, argv, "talker");

	// Construct and init a new RosComponent class shared pointer
	pRosComp.reset(new RTOS::RosComponent());

	// Create a new Xenomai RT POSIX thread
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
