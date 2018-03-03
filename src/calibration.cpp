#include "rtos_util.hpp"
#include "RaspiLowLevel.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

#define PERIOD_MICROSECS 10000 //10 millisecs

boost::shared_ptr<RASPI::RaspiLowLevel> pRaspiLLHandle;
bool calibration_finish = false;

void * calibration_task( void * dummy )
{
	int i = 0;
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
			pRaspiLLHandle->fetch_data_from_stm32(&pRaspiLLHandle->ins_data, true);
			for (ite = 0; ite < 6; ite++)
				printf("%f ", pRaspiLLHandle->ins_bias.at(ite));
			printf("\n");
			for (ite = 0; ite < 9; ite++)
				printf("%f ", pRaspiLLHandle->ins_data.at(ite));
			printf("\n");

			printf("-- [INFO]: Finish calibration.\n");
			calibration_finish = true;
			RTOS::ThreadRunning = 0;
		}
	}
	return 0;
}

void * magnetometer_calibration_task( void * dummy) {

	int i = 0;
	while ( RTOS::ThreadRunning ) {
		RTOS::WaitPeriodicPosixTask();

		if (i < 6000) {
			i++;
			pRaspiLLHandle->calibrate_magnetometer();
		}
		else {
			int ite = 0;
			pRaspiLLHandle->fetch_data_from_stm32(&pRaspiLLHandle->ins_data, true);

			for (ite = 0; ite < 3; ite++) 
				printf("%f ", pRaspiLLHandle->ins_data.at(ite + 6));
			printf("\n");

			printf("-- [INFO]: Finish calibration.\n");
			calibration_finish = true;
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
		printf("-- [INFO]: Begin accel and gyro calibration procedure.\n");
		err = RTOS::CreatePosixTask( "DemoPosix", 1/*Priority*/, 16/*StackSizeInKo*/, PERIOD_MICROSECS/*PeriodMicroSecs*/, calibration_task );
		if ( err!=0 ) {
			printf( "-- [ERROR]: Init task error (%d)!\n",err );
		}
		else {
			while (!calibration_finish) {
				sleep(0.5);
			}
			
			std::ofstream accel_calibration_file;
			std::ofstream gyro_calibration_file;
			std::ofstream log_file;

			printf("-- [INFO]: Writing calibration data to file.\n");
			
			accel_calibration_file.open("/home/pi/accel_calib.txt");
			gyro_calibration_file.open("/home/pi/gyro_calib.txt");
			log_file.open("/home/pi/imu_log.txt", std::ios::out | std::ios::app);

			// Writing accelerometer calibration data to file.
			accel_calibration_file << pRaspiLLHandle->ins_bias.at(0) << " " << pRaspiLLHandle->ins_bias.at(1) << " " << pRaspiLLHandle->ins_bias.at(2) << "\n";

			// Writing gyroscope calibration data to file.
			gyro_calibration_file << pRaspiLLHandle->ins_bias.at(3) << " " << pRaspiLLHandle->ins_bias.at(4) << " " << pRaspiLLHandle->ins_bias.at(5) << "\n";

			// Loging newly accquired calibration data for future reference.
			for (int i = 0; i < 6; i++)
				log_file << pRaspiLLHandle->ins_bias.at(i) << " ";

			accel_calibration_file.close();
			gyro_calibration_file.close();
			log_file.close();

			printf("-- [INFO]: Finish writing to files.\n");
		}


		// 
		printf("-- [INFO]: Begin magnetometer calibration procedure.\n");
		RTOS::ThreadRunning = 0;
		calibration_finish = false;

		err = RTOS::CreatePosixTask( "MagnetCalibTask", 1, 16, PERIOD_MICROSECS, magnetometer_calibration_task );

		if ( err != 0 ) {
			printf( "-- [ERROR]: Init task error (%d)!\n",err );
		}
		else {
			while (!calibration_finish) {
				sleep(0.5);
			}

			std::ofstream magnetometer_calibration_file;
			std::ofstream log_file;

			printf("-- [INFO]: Writing calibration data to file.\n");
			
			magnetometer_calibration_file.open("/home/pi/magnetometer_calib.txt");
			log_file.open("/home/pi/imu_log.txt", std::ios::out | std::ios::app);

			// Writing magnetometer calibration data to file
			magnetometer_calibration_file << pRaspiLLHandle->ins_bias.at(6) << " " << pRaspiLLHandle->ins_bias.at(7) << " " << pRaspiLLHandle->ins_bias.at(8) << "\n";

			// Loging newly accquired calibration data for future reference.
			for (int i = 0; i < 3; i++)
				log_file << pRaspiLLHandle->ins_bias.at(i + 6) << " ";
			log_file << "\n";

			printf("-- [INFO]: Finish writing to files.\n");
		}
	}
	
	return 0;
	
}
