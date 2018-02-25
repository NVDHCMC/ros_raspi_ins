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
boost::shared_ptr<RASPI::RaspiLowLevel> pRaspiLLHandle;

void * MySimpleTask( void * dummy )
{
	int i = 0;
	std::vector<float> ins_data (10, 0);
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		//pRaspiLLHandle->fetch_data_from_stm32(&ins_data);
		pRosComp->send_data();
		ResultIncValue++;
	}
	return 0;
}

int main(int argc, char ** argv) {
	// Initialize ros with input params
	ros::init(argc, argv, "talker");

	// Construct and init a new RosComponent class shared pointer
	pRosComp.reset(new RTOS::RosComponent());
	pRaspiLLHandle.reset(new RTOS::RaspiLowLevel());

	pRaspiLLHandle->init_spi();
	pRaspiLLHandle->init_stm32();
	pRaspiLLHandle->pair_stm32();

	// Create a new Xenomai RT POSIX thread
	int err;
	
	err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 16/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
	
	if ( err!=0 )
	{
		printf( "Init task error (%d)!\n",err );
	}
	else
	{
		do {
			std::cout << "Press ESC to stop."; 
		} while (std::cin.get() != 27); 

		RTOS::ThreadRunning = 0;
	}
	
	return 0;
	
}
