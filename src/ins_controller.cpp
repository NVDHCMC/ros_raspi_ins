#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"
#include "RosComponent.hpp"
#include "RaspiLowLevel.hpp"
#include "ins_controller/Ins.h"
#include <sstream>

uint32_t ResultIncValue = 0;
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
		pRaspiLLHandle->fetch_data_from_stm32(&pRaspiLLHandle->ins_data);
		//pRosComp->send_data();
		ResultIncValue++;
	}
	return 0;
}

int main(int argc, char ** argv) {
	
	printf("Simple periodic thread using Posix.\n");
#if defined( __XENO__ ) || defined( __COBALT__ )
	printf("Version compiled for Xenomai 2 or 3.\n");
	mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

	// Initialize ros with input params
	//ros::init(argc, argv, "talker");

	// Construct and init a new RosComponent class shared pointer
	//pRosComp.reset(new RTOS::RosComponent());
	pRaspiLLHandle.reset(new RASPI::RaspiLowLevel());

	// Initialize SPI periph and pairing with stm32
	pRaspiLLHandle->init_spi();
	pRaspiLLHandle->pair_stm32();
	//pRaspiLLHandle->fetch_data_from_stm32(&pRaspiLLHandle->ins_data);

	// Create a new Xenomai RT POSIX thread
	if (pRaspiLLHandle->pair_stm32()) {
		int err;
		err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 16/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
	}
	std::cin.get();
	if ( err!=0 )
	{
		printf( "Init task error (%d)!\n",err );
	}
	else
	{
		printf("Press ESC to stop.\n"); 
		while (std::cin.get() != 27)
		{
			sleep(0.5);
		}

		RTOS::ThreadRunning = 0;
	}
	
	return 0;
	
}
