#include "KalmanFilter.hpp"

namespace RTOS 
{
	KalmanFilter::KalmanFilter(): 
	{
		this->x = 0.0f;

		this->dt = 1.0f/100.0f;
		this->A = 1.0f;
		this->B = this->dt;
		this->Q = 241.0f;
		this->R = 4.0f;
		this->P = 0.0f;
		this->K = 0.0f;
	}

	KalmanFilter::~KalmanFilter() {}

	void KalmanFilter::init(void)
	{
		this->x0 = 0.0;
		this->P0 = 0.0;
		this->x_hat = this->x0;
	}

	void KalmanFilter::init(float x0)
	{
		this->x0 = x0;
		this->P0 = 0.0;
		this->x_hat = this->x0;
	}

	void KalmanFilter::update(float y, float u)
	{
		this->x_hat_new = this->A * this->x_hat;
		this->P = this->P + this->Q;
		this->K = this->P/(this->P + this->R);
		this->x_hat_new += this->K * (y - this->x_hat_new);
		this->P = (1.0f - this->K)*this->P;
		this->x_hat = this->x_hat_new;
	}	
}