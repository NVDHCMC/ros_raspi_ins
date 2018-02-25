#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/timerfd.h>
#include <sched.h>
#include <boost/shared_ptr.hpp>



namespace RTOS {
	extern char ThreadRunning;
	int CreatePosixTask( char * TaskName, int Priority, \
	                    int StackSizeInKo, unsigned int PeriodMicroSecs, void * (*pTaskFunction)(void *) );
	
	void WaitPeriodicPosixTask( );
}
