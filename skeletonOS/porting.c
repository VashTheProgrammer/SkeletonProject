// return usec
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint64_t getMicrosecTime()
{
 struct timespec spec;

 clock_gettime(CLOCK_REALTIME, &spec);

 /* convert to microseconds */
 return ((uint64_t)spec.tv_sec * 1000000) + ((uint64_t)spec.tv_nsec / 1000U);
}

uint64_t getMillisTime()
{
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);

	/* convert to milliseconds */
	return ((uint64_t)spec.tv_sec * 1000) + ((uint64_t)spec.tv_nsec / 1000000U);
}

uint32_t rnd(void)
{
	return rand();
}


void hardwareSchedulerRun()
{
    usleep(1000);
}