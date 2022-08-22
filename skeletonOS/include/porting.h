#ifndef PORTING_H_
#define PORTING_H_

#include <stdint.h>

extern uint64_t getMicrosecTime();

extern void hardwareSchedulerRun();

#endif // !FSM_H_