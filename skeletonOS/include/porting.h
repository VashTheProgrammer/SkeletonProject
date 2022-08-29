#ifndef PORTING_H_
#define PORTING_H_

#include <stdint.h>

extern uint64_t getMicrosecTime();

extern uint64_t getMillisTime();

extern uint32_t rnd();

extern void hardwareSchedulerRun();

#endif // !FSM_H_