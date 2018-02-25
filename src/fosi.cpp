#include "rtos_util.hpp"

#define PERIOD_MICROSECS 5000 //5 millisecs
#define START_DELAY_SECS 1 //1sec
#define PIN RPI_GPIO_P1_11
namespace RTOS {
	pthread_t MyPosixThread;
	int TimerFdForThread = -1;
	char ThreadRunning = 0;

	int CreatePosixTask( char * TaskName, int Priority, int StackSizeInKo, unsigned int PeriodMicroSecs, void * (*pTaskFunction)(void *) )
	{
		pthread_attr_t ThreadAttributes;
		int err = pthread_attr_init(&ThreadAttributes);
		if ( err )
		{
			printf("pthread attr_init() failed for thread '%s' with err=%d\n", TaskName, err );
			return -10;
		}
	#ifdef __COBALT__
		err = pthread_attr_setinheritsched( &ThreadAttributes, PTHREAD_EXPLICIT_SCHED );
		if ( err )
		{
			printf("pthread set explicit sched failed for thread '%s' with err=%d\n", TaskName, err );
			return -11;
		}
	#endif
		err = pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_DETACHED /*PTHREAD_CREATE_JOINABLE*/);
		if ( err )
		{
			printf("pthread set detach state failed for thread '%s' with err=%d\n", TaskName, err );
			return -12;
		}
	#if defined(__XENO__) || defined(__COBALT__)
		err = pthread_attr_setschedpolicy(&ThreadAttributes, SCHED_FIFO);
		if ( err )
		{
			printf("pthread set scheduling policy failed for thread '%s' with err=%d\n", TaskName, err );
			return -13;
		}
		struct sched_param paramA = { .sched_priority = Priority };
		err = pthread_attr_setschedparam(&ThreadAttributes, &paramA);
		if ( err )
		{
			printf("pthread set priority failed for thread '%s' with err=%d\n", TaskName, err );
			return -14;
		}
	#endif
		if ( StackSizeInKo>0 )
		{
			err = pthread_attr_setstacksize(&ThreadAttributes, StackSizeInKo*1024);
			if ( err )
			{
				printf("pthread set stack size failed for thread '%s' with err=%d\n", TaskName, err );
				return -15;
			}
		}

		// calc start time of the periodic thread
		struct timespec start_time;
	#ifdef __XENO__
		if ( clock_gettime( CLOCK_REALTIME, &start_time ) )
	#else
		if ( clock_gettime( CLOCK_MONOTONIC, &start_time ) )
	#endif
		{
			printf( "Failed to call clock_gettime\n" );
			return -20;
		}
		/* Start one seconde later from now. */
		start_time.tv_sec += START_DELAY_SECS ;
		
		// if a timerfd is used to make thread periodic (Linux / Xenomai 3),
		// initialize it before launching thread (timer is read in the loop)
	#ifndef __XENO__ 
		struct itimerspec period_timer_conf;
		TimerFdForThread = timerfd_create(CLOCK_MONOTONIC, 0);
		if ( TimerFdForThread==-1 )
		{
			printf( "Failed to create timerfd for thread '%s'\n", TaskName);
			return -21;
		}
		period_timer_conf.it_value = start_time;
		period_timer_conf.it_interval.tv_sec = 0;
		period_timer_conf.it_interval.tv_nsec = PeriodMicroSecs*1000;
		if ( timerfd_settime(TimerFdForThread, TFD_TIMER_ABSTIME, &period_timer_conf, NULL) )
		{
			printf( "Failed to set periodic tor thread '%s' with errno=%d\n", TaskName, errno);
			return -22;
		}
	#endif
		
		ThreadRunning = 1;
		err = pthread_create( &MyPosixThread, &ThreadAttributes, (void *(*)(void *))pTaskFunction, (void *)NULL );
		if ( err )
		{
			printf( "Failed to create thread '%s' with err=%d !!!!!\n", TaskName, err );
			return -1;
		}
		else
		{
			// make thread periodic for Xenomai 2 with pthread_make_periodic_np() function.
	#ifdef __XENO__ 
			struct timespec period_timespec;
			period_timespec.tv_sec = 0;
			period_timespec.tv_nsec = PeriodMicroSecs*1000;
			if ( pthread_make_periodic_np(MyPosixThread, &start_time, &period_timespec)!=0 )
			{
				printf("Xenomai make_periodic failed for thread '%s' with err=%d\n", TaskName, err);
				return -30;
			}
	#endif

			pthread_attr_destroy(&ThreadAttributes);
	#ifdef __XENO__ 
			err = pthread_set_name_np( MyPosixThread, TaskName );
	#else
			err = pthread_setname_np( MyPosixThread, TaskName );
	#endif
			if ( err )
			{
				printf("pthread set name failed for thread '%s', err=%d\n", TaskName, err );
				return -40;
			}
			
			printf( "Created thread '%s' period=%dusecs ok.\n", TaskName, PeriodMicroSecs );
			return 0;
		}
	}

	void WaitPeriodicPosixTask( )
	{
		int err = 0;
	#ifdef __XENO__
		unsigned long overruns;
		err = pthread_wait_np(&overruns);
		if (err || overruns)
		{
			printf( "Xenomai wait_period failed for thread: err=%d, overruns=%lu\n", err, overruns );
		}
	#else
		uint64_t ticks;
		err = read(TimerFdForThread, &ticks, sizeof(ticks));
		if ( err<0 )
		{
			printf( "TimerFd wait period failed for thread with errno=%d\n", errno );
		}
		if ( ticks>1 )
		{
			printf( "TimerFd wait period missed for thread: overruns=%lu\n", (long unsigned int)ticks );
		}
	#endif
	}
}
