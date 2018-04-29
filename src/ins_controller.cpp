#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"
#include "RosComponent.hpp"
//#include "RaspiLowLevel.hpp"
#include "ins_controller/Ins.h"
#include "MahonyAHRS.h"
#include "mpu9255.hpp"
#include <sstream>
#include <fstream>

#define PERIOD_MICROSECS 10000 //10 millisecs

boost::shared_ptr<RTOS::RosComponent> pRosComp;
boost::shared_ptr<Mahony> pMahonyFilter;
boost::shared_ptr<SENSOR::mpu9255> pMPU9255;

void * MySimpleTask( void * dummy )
{
	char ID = 0x00;
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		
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
	pMPU9255.reset(new SENSOR::mpu9255());

	// Initialize SPI periph and pairing with stm32
	bool ret = pMPU9255->init();
	if (!ret)
	{
		printf("Failed to initialize MPU9255,\n");
		return -1;
	}

	pMahonyFilter->begin(100);

	// Create a new Xenomai RT POSIX thread
	ID = pMPU9255->get_id();
	if (ID != I_AM_MPU9255)
	{
		printf("Not recognized as MPU9255.\n");
		return -1;
	}




	sleep(0.5);
	int err;
	std::cin.get();
	err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 64000/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
	if ( err!=0 ) {
		printf( "Init task error (%d)!\n",err );
		return -1;
	}
	else {
	}
	
	return 0;
	
}
