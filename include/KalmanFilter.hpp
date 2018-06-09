

#ifndef KALMAN_FILTER_HPP
#define KALMAN_FILTER_HPP

#include "ros/ros.h"
#include <stdint.h>
#include <iostream>
#include <vector>


namespace RTOS
{
	class KalmanFilter
	{
	public:
		KalmanFilter();
		~KalmanFilter();

		void init(void);
		void init(float x0);
		void update();
		float x;

	private:
		float dt;
		float A;
		float B;
		float Q;
		float R;
		float P;
		float P0;
		float x0;
		float K;
		float x_hat;
		float x_hat_new;
	};
}

#endif /* Kalman Filter */