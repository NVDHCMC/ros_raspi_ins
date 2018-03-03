#include "rtos_util.hpp"
#include "RaspiLowLevel.hpp"
#include <sstream>
#include <fstream>

#define PERIOD_MICROSECS 10000 //10 millisecs

boost::shared_ptr<RASPI::RaspiLowLevel> pRaspiLLHandle;

void * calibration_task( void * dummy )
{
	int i = 0;
	std::vector<float> ins_data (10, 0);

	while( RTOS::ThreadRunning )
	{
		RTOS::WaitPeriodicPosixTask( );
		if (i < 1000) {
			i++;
			pRaspiLLHandle->calibrate();
		}
		else {
			int ite = 0;
			// Fetch data with calibration
			pRaspiLLHandle->fetch_data_from_stm32(pRaspiLLHandle->ins_data, true);
			for (ite = 0; ite < 6; ite++)
				printf("%f ", pRaspiLLHandle->ins_bias.at(ite));
			printf("\n");
			for (ite = 0; ite < 9; ite++)
				printf("%f ", pRaspiLLHandle->ins_data.at(ite));
			printf("\n");

			printf("-- [INFO]: Finish calibration.\n");

			RTOS::ThreadRunning = 0;
		}
	}
	return 0;
}

int main(int argc, char ** argv) {
	
	printf("-- [INFO]: Simple periodic thread using Posix skin.\n");
#if defined( __XENO__ ) || defined( __COBALT__ )
	printf("-- [INFO]: Version compiled for Xenomai 2 or 3.\n");
	mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

	pRaspiLLHandle.reset(new RASPI::RaspiLowLevel());
	//pMahonyFilter.reset(new Mahony());

	// Initialize SPI periph and pairing with stm32
	pRaspiLLHandle->init_spi();

	// Create a new Xenomai RT POSIX thread
	sleep(0.5);

	if (pRaspiLLHandle->pair_stm32()) {
		int err;
		std::cin.get();
		printf("-- [INFO]: Start calibration.\n");
		err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 16/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, calibration_task );
		if ( err!=0 ) {
			printf( "Init task error (%d)!\n",err );
		}
		else {
			printf("Press ESC to stop.\n"); 
			while (std::cin.get() != 27)
			{
				sleep(0.5);
			}

			RTOS::ThreadRunning = 0;
		}
	}
	
	return 0;
	
}
