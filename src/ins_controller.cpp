#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"
#include "RosComponent.hpp"
//#include "RaspiLowLevel.hpp"
#include "mpu9255.hpp"
#include "ins_controller/Ins.h"
#include "MahonyAHRS.h"
#include <sstream>
#include <fstream>

#define PERIOD_MICROSECS 10000 //10 millisecs

typedef boost::shared_ptr<RTOS::RosComponent> pRosComponent;
boost::shared_ptr<RTOS::RosComponent> pRosComp;
boost::shared_ptr<Mahony> pMahonyFilter;
boost::shared_ptr<SENSOR::mpu9255> pMPU9255;
bool flags = false;

void * MySimpleTask( void * dummy )
{
	uint8_t ID = 0x00;
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		ID = pMPU9255->getID();
		if (ID != SENSOR::I_AM_MPU9255)
		{
			printf("Not recognized as MPU9255.\n");
			RTOS::ThreadRunning = 0;
		}
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
	ros::init(argc, argv, "talker");

	// Construct and init a new RosComponent class shared pointer
	pRosComp.reset(new RTOS::RosComponent());
	pMahonyFilter.reset(new Mahony());
	pMPU9255.reset(new mpu9255());

	// Initialize SPI periph and pairing with stm32
	pMahonyFilter->begin(100);

	// Create a new Xenomai RT POSIX thread
	sleep(0.5);
	char opt;
	if (pRaspiLLHandle->pair_stm32()) {
		int err;
		std::cin.get();
		err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 64000/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
		if ( err!=0 ) {
			printf( "Init task error (%d)!\n",err );
		}
		else {
		}
	}
	
	return 0;
	
}
