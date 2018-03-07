#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rtos_util.hpp"
#include "RosComponent.hpp"
#include "RaspiLowLevel.hpp"
#include "ins_controller/Ins.h"
#include "MahonyAHRS.h"
#include <sstream>

#define PERIOD_MICROSECS 10000 //10 millisecs

typedef boost::shared_ptr<RTOS::RosComponent> pRosComponent;
boost::shared_ptr<RTOS::RosComponent> pRosComp;
boost::shared_ptr<RASPI::RaspiLowLevel> pRaspiLLHandle;
boost::shared_ptr<Mahony> pMahonyFilter;
bool flags = false;
FILE * pFile;
void * MySimpleTask( void * dummy )
{
	int i = 0;
	std::vector<float> RPY (3, 0);
	uint32_t ResultIncValue = 0;
	//float buffer[90] = {0.0f};
	bool full = false;
	uint32_t pack = 0;
	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		if (i < 100) {
			i++;
			pRaspiLLHandle->calibrate();
		}
		else {
			pRaspiLLHandle->fetch_data_from_stm32(&pRaspiLLHandle->ins_data, true);
			pMahonyFilter->update(pRaspiLLHandle->ins_data.at(3), pRaspiLLHandle->ins_data.at(4), pRaspiLLHandle->ins_data.at(5), 
				pRaspiLLHandle->ins_data.at(0), pRaspiLLHandle->ins_data.at(1), pRaspiLLHandle->ins_data.at(2),	 
				pRaspiLLHandle->ins_data.at(7), pRaspiLLHandle->ins_data.at(6), -pRaspiLLHandle->ins_data.at(8));
			RPY.at(0) = pMahonyFilter->getRoll();
			RPY.at(1) = pMahonyFilter->getPitch();
			RPY.at(2) = pMahonyFilter->getYaw();
			printf("%f %f %f %f \n", RPY.at(0), RPY.at(1), RPY.at(2), pRaspiLLHandle->ins_data.at(6)*pRaspiLLHandle->ins_data.at(6) + pRaspiLLHandle->ins_data.at(7)*pRaspiLLHandle->ins_data.at(7) + pRaspiLLHandle->ins_data.at(8)*pRaspiLLHandle->ins_data.at(8));
			//pRosComp->send_data();
			fprintf(pFile, "%f %f %f %f %f %f %f %f %f\n", pRaspiLLHandle->ins_data.at(0), pRaspiLLHandle->ins_data.at(1), pRaspiLLHandle->ins_data.at(2), 
				pRaspiLLHandle->ins_data.at(3), pRaspiLLHandle->ins_data.at(4), pRaspiLLHandle->ins_data.at(5),
				pRaspiLLHandle->ins_data.at(7), pRaspiLLHandle->ins_data.at(6), -pRaspiLLHandle->ins_data.at(8));
			ResultIncValue++;
			if (ResultIncValue == 10000) {
				RTOS::ThreadRunning = 0;
			}
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
	pRaspiLLHandle.reset(new RASPI::RaspiLowLevel());
	pMahonyFilter.reset(new Mahony());

	// Initialize SPI periph and pairing with stm32
	pRaspiLLHandle->init_spi();

	pMahonyFilter->begin(100);

	pFile = fopen("/home/pi/data.txt", "w");

	// Create a new Xenomai RT POSIX thread
	sleep(0.5);
	char opt;
	if (pRaspiLLHandle->pair_stm32()) {
		int err;
		std::cin.get();
		err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 32/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, MySimpleTask );
		if ( err!=0 ) {
			printf( "Init task error (%d)!\n",err );
		}
		else {
			while (!flags) {
				//sleep(1);
			}
			printf("-- [INFO]: Done logging\n");
			fclose(pFile);
		}
	}
	
	return 0;
	
}
