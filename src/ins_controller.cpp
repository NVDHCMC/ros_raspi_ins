#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"
#include "RosComponent.hpp"
//#include "RaspiLowLevel.hpp"
#include "ipc_communication.hpp"
#include "ins_controller/Ins.h"
#include "MadgwickAHRS.h"
#include "mpu9255.hpp"
#include "controller.hpp"
#include <math.h>
#include <sstream>
#include <fstream>

#define PERIOD_MICROSECS 10000 //10 millisecs

boost::shared_ptr<RTOS::RosComponent> pRosComp;
//boost::shared_ptr<Mahony> pMahonyFilter;
boost::shared_ptr<Madgwick> pMahonyFilter;
boost::shared_ptr<SENSOR::mpu9255> pMPU9255;
boost::shared_ptr<RASPI::controller> pCTRL;
boost::shared_ptr<RTOS::ipc_com> pIPC;
FILE * pFd;

void * MySimpleTask( void * dummy )
{
	char ID = 0x00;
	std::vector<float> RPY (3, 0);
	char temp[32];
	char data[255];
	struct timespec ts;
	struct timespec r_ts;
	float yaw = 0.0f;
	int i = 0;
	pCTRL->control_params.at(0) = 0.0f;
	pCTRL->control_params.at(1) = 0.0;//0.3333333f;
	ts.tv_sec = 0;
	ts.tv_nsec = 500000; /* 0.5 ms */
	r_ts.tv_sec = 0;
	r_ts.tv_nsec = 2000000; /* 2 ms */
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask();
		pMPU9255->get_data();
		pMahonyFilter->update(pMPU9255->ins_data.at(3), pMPU9255->ins_data.at(4), pMPU9255->ins_data.at(5), 
							  pMPU9255->ins_data.at(0), pMPU9255->ins_data.at(1), pMPU9255->ins_data.at(2),	 
							  pMPU9255->ins_data.at(7), pMPU9255->ins_data.at(6), -pMPU9255->ins_data.at(8));
		RPY.at(0) = pMahonyFilter->getRoll();
		RPY.at(1) = pMahonyFilter->getPitch();
		RPY.at(2) = pMahonyFilter->getYaw();
		//yaw = atan2(pMPU9255->ins_data.at(6), pMPU9255->ins_data.at(7))*57.29747f;
		//pIPC->send(temp, 12);
		//printf("%f %f %f %f \n", RPY.at(0), RPY.at(1), RPY.at(2), pMPU9255->ins_data.at(6)*pMPU9255->ins_data.at(6) + pMPU9255->ins_data.at(7)*pMPU9255->ins_data.at(7) + pMPU9255->ins_data.at(8)*pMPU9255->ins_data.at(8));
		//printf("%f %f %f %f %f\n", yaw, RPY.at(2), pMPU9255->ins_data.at(6), pMPU9255->ins_data.at(7), pMPU9255->ins_data.at(8));
		//clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);

		if (i < 100) {
			i++;
		}
		else {
			pCTRL->control_params.at(0) = 300.0f;
		}

		memset(temp, 0x00, 32);
		temp[0] = 0xff;
		temp[1] = 0x01;
		pCTRL->test_send(temp ,32);
		clock_nanosleep(CLOCK_REALTIME, 0, &r_ts, NULL);
		pCTRL->get_pos();
		printf("%f %f %f\n", pCTRL->current_state.at(0), pCTRL->current_state.at(1), pCTRL->current_pos.at(0));
		pCTRL->estimate_pos();
		if (pCTRL->current_pos.at(0) > 600) // millimeters
		{
			pCTRL->control_params.at(0) = 0.0;
		}
		//
		// Main controller algorithm
		//
		clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
		pCTRL->calculate_speed_for_motors();
		pCTRL->serialization();
		pCTRL->test_send(pCTRL->send, 32);
		clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
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
	pMahonyFilter.reset(new Madgwick());
	pMPU9255.reset(new SENSOR::mpu9255());
	pCTRL.reset(new RASPI::controller(0x0c));
	pIPC.reset(new RTOS::ipc_com(0x00, 0x01));

	// Initialize SPI periph and pairing with stm32
	bool ret = pMPU9255->init();
	if (!ret)
	{
		printf("-- [INFO] Failed to initialize MPU9255,\n");
		return -1;
	}

//	pMahonyFilter->begin(100);

	// Create a new Xenomai RT POSIX thread
	char ID;
	ID = pMPU9255->get_id();
	if (ID != I_AM_MPU9255)
	{
		printf("-- [INFO] Not recognized as MPU9255.\n");
		return -1;
	}

	// Test i2c
	if (!pCTRL->init_i2c())
	{
		printf("-- [INFO] Could not initialize I2C.\n");
		return -1;
	}

	pFd = fopen("/home/pi/log_data1.txt", "w");
	int err;
	printf("-- [INFO] Press any key to start\n");
	std::cin.get();
	err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 64000/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
	if ( err!=0 ) {
		printf( "-- [INFO] Init task error (%d)!\n",err );
		return -1;
	}
	else {
		printf("Press ESC to stop.\n");
		while (std::cin.get() != 27)
		{
			sleep(0.5);
		}

		RTOS::ThreadRunning = 0;
	}

	fclose(pFd);

	return 0;

}
