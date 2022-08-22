// return usec
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

uint64_t getMicrosecTime()
{
 struct timespec spec;

 clock_gettime(CLOCK_REALTIME, &spec);

 /* convert to microseconds */
 return ((uint64_t)spec.tv_sec * 1000000) + ((uint64_t)spec.tv_nsec / 1000U);
}

void hardwareSchedulerRun()
{
    usleep(1000);
}