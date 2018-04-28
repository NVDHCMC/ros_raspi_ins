
#ifndef MPU9255_HPP
#define MPU9255_HPP

#include "ros/ros.h"
#include <bcm2835.h>
#include <stdint.h>
#include <iostream>
#include <vector>

#define MPU9255_WHO_AM_I_ADDR          0x75  /* device identification register */				// Checked
#define MPU9255_CONFIG_ADDR        	   0x1a  /* Config register */ 								// Checked
#define MPU9255_GYRO_CONFIG_ADDR       0x1b  /* Gyro configuration register */					// Checked
#define MPU9255_ACCEL_CONFIG_ADDR      0x1c  /* Accelerometer configuration register */ 		// Checked
#define MPU9255_ACCEL_CONFIG_2_ADDR    0x1d  /* Accelerometer configuration register 2 */ 		// Checked
#define MPU9255_LP_ACCEL_CTRL_ADDR     0x1e  /* Low-power mode accelerometer  					// Checked
											  * output rate configuration register */
#define MPU9255_WOM_THR_ADDR           0x1f  /* Wake-on-motion threshold register */			// Checked
#define MPU9255_FIFO_EN_ADDR           0x23  /* FIFO enable register */							// Checked
#define MPU9255_I2C_MASTER_CTRL 	   0x24  /* I2C master control register */ 					// Checked

#define MPU9255_INT_PIN_CFG_ADDR       0x37  /* Interrupt pin configuration register */			// Checked
#define MPU9255_INT_ENABLE_ADDR        0x38  /* Interrupt enable register */					// Checked
#define MPU9255_INT_STATUS_ADDR        0x3a  /* Interrupt status register */					// Checked
#define MPU9255_ACCEL_OUT_X_H_ADDR     0x3b  /* Accelerometer X output L register */			// Checked
#define MPU9255_ACCEL_OUT_X_L_ADDR     0x3c  /* Accelerometer X output H register */			// Checked
#define MPU9255_ACCEL_OUT_Y_H_ADDR     0x3d  /* Accelerometer Y output L register */			// Checked
#define MPU9255_ACCEL_OUT_Y_L_ADDR     0x3e  /* Accelerometer Y output H register */			// Checked
#define MPU9255_ACCEL_OUT_Z_H_ADDR     0x3f  /* Accelerometer Z output L register */			// Checked
#define MPU9255_ACCEL_OUT_Z_L_ADDR     0x40  /* Accelerometer Z output H register */			// Checked
#define MPU9255_TEMP_OUT_H_ADDR        0x41  /* Temperature Z output L register */				// Checked
#define MPU9255_TEMP_OUT_L_ADDR        0x42  /* Temperature Z output H register */				// Checked
#define MPU9255_GYRO_OUT_X_H_ADDR      0x43  /* Gyroscope X output L register */				// Checked
#define MPU9255_GYRO_OUT_X_L_ADDR      0x44  /* Gyroscope X output H register */				// Checked
#define MPU9255_GYRO_OUT_Y_H_ADDR      0x45  /* Gyroscope Y output L register */				// Checked
#define MPU9255_GYRO_OUT_Y_L_ADDR      0x46  /* Gyroscope Y output H register */				// Checked
#define MPU9255_GYRO_OUT_Z_H_ADDR      0x47  /* Gyroscope Z output L register */				// Checked
#define MPU9255_GYRO_OUT_Z_L_ADDR      0x48  /* Gyroscope Z output H register */				// Checked
#define MPU9255_SIGNAL_PATH_RST_ADDR   0x68  /* Signal path reset register */					// Checked
#define MPU9255_MOT_DETECT_CTRL_ADDR   0x69  /* Motion detection controller register */			// Checked
#define MPU9255_USER_CTRL_ADDR         0x6a  /* User controller register */						// Checked
#define MPU9255_PWR_MGMT_1_ADDR        0x6b  /* Power management 1 register */					// Checked
#define MPU9255_PWR_MGMT_2_ADDR        0x6c  /* Power management 2 register */					// Checked

// Not checked
/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/
/* Read/Write command */ // Not checked
#define READWRITE_CMD                           ((uint8_t)0x80)
/* Multiple byte read/write command */ // Not checked
#define MULTIPLEBYTE_CMD                        ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
// Not checked
#define DUMMY_BYTE                              ((uint8_t)0x00)

#define I_AM_MPU9255                 ((uint8_t)0x73) 											// Checked

/** @defgroup Power_Management_selection
  * @{
  */
#define MPU9255_MODE_RESET_ALL 		 ((uint8_t)0x80)
#define MPU9255_MODE_SLEEP       	 ((uint8_t)0x40)
#define MPU9255_MODE_CYCLE           ((uint8_t)0x20)
#define MPU9255_MODE_STANDBY 		 ((uint8_t)0x10)
#define MPU9255_MODE_PD_PTAT 		 ((uint8_t)0x08)
/**
  * @}
  */

/** @defgroup Clock_source_select 																// Checked
  * @{
  */
#define MPU9255_CLKSEL_20MHZ_INTERNAL_OSCILLATOR ((uint8_t)0x00)
#define MPU9255_CLKSEL_AUTO_SELECT 	 ((uint8_t)0x01)
#define MPU9255_CLKSEL_STOP 		 ((uint8_t)0x07)
/**
  * @}
  */

/** @defgroup Axes_Selection 																	// Checked
  * @{
  */
#define MPU9255_ACCEL_X_ENABLE      ((uint8_t)0xe0)
#define MPU9255_ACCEL_Y_ENABLE      ((uint8_t)0xd0)
#define MPU9255_ACCEL_Z_ENABLE      ((uint8_t)0xc8)
#define MPU9255_GYRO_X_ENABLE       ((uint8_t)0xc4)
#define MPU9255_GYRO_Y_ENABLE       ((uint8_t)0xc2)
#define MPU9255_GYRO_Z_ENABLE 		((uint8_t)0xc1)
/**
  * @}
  */

/** @defgroup Bandwidth_Selection 																// Checked
  * @{
  */
#define MPU9255_BANDWIDTH_0         ((uint8_t)0x00)
#define MPU9255_BANDWIDTH_1         ((uint8_t)0x01)
#define MPU9255_BANDWIDTH_2         ((uint8_t)0x02)
#define MPU9255_BANDWIDTH_3         ((uint8_t)0x03)
#define MPU9255_BANDWIDTH_4         ((uint8_t)0x04)
#define MPU9255_BANDWIDTH_5         ((uint8_t)0x05)
#define MPU9255_BANDWIDTH_6         ((uint8_t)0x06)
#define MPU9255_BANDWIDTH_7         ((uint8_t)0x07)
/**
  * @}
  */

/** @defgroup Full_Scale_Selection 																// Checked
  * @{
  */
#define MPU9255_FULLSCALE_250       ((uint8_t)0x00)
#define MPU9255_FULLSCALE_500       ((uint8_t)0x08)
#define MPU9255_FULLSCALE_1000      ((uint8_t)0x10)
#define MPU9255_FULLSCALE_2000 		((uint8_t)0x18)
/**
  * @}
  */

/** @defgroup Gyro_Filter_choice 																// Checked
  * @{
  */
#define MPU9255_GYRO_FILTER_1 		((uint8_t) 0x03)
#define MPU9255_GYRO_FILTER_2 		((uint8_t) 0x02)
#define MPU9255_GYRO_FILTER_3 		((uint8_t) 0x00)
/**
  * @}
  */

/** @defgroup Accelerometer_Full_Scale_Selection 												// Checked
  * @{
  */
#define MPU9255_FULLSCALE_2G      	((uint8_t)0x00)         /*!< gyroscope sensitivity with 250 dps full scale [DPS/LSB]  */
#define MPU9255_FULLSCALE_4G 		((uint8_t)0x08)        /*!< gyroscope sensitivity with 500 dps full scale [DPS/LSB]  */
#define MPU9255_FULLSCALE_8G 		((uint8_t)0x10)        /*!< gyroscope sensitivity with 2000 dps full scale [DPS/LSB] */
#define MPU9255_FULLSCALE_16G		((uint8_t)0x18)
/**
  * @}
  */

/** @defgroup Accel_Filter_choice 																// Checked
  * @{
  */
#define MPU9255_ACCEL_FILTER_0 		((uint8_t) 0x80)
#define MPU9255_ACCEL_FILTER_1 		((uint8_t) 0x00)

namespace SENSOR {
	class mpu9255
	{
	public:
		mpu9255();
		~mpu9255();
		bool write_reg(uint8_t reg_cmd, uint8_t REG_ADDR, int num_byte);
		void read_reg(uint8_t REG_ADDR, uint8_t * pData, int num_byte);
		uint8_t get_id();
		//void calibrate();
		//void calibrate_magnetometer();
		std::vector<float> ins_data;
		std::vector<float> ins_bias;
	private:
		bool init;
		bool spi_init;

		std::vector<float> acc_err;
		std::vector<uint16_t> buffer;
		std::vector<float> mag_max_value;
		std::vector<float> mag_min_value;
	};
}

#endif /* MPU9255_HPP */
