#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <inttypes.h>

/* Process control block - 
 * holding all process relevant informations 
*/
struct pcb
{
	uint16_t pid;			 /* ID of the proces */
	uint64_t period;		 /* process period */
	uint64_t elapsed;		 /* process time elapsed */
	uint32_t attached;		 /* 1 if attached to processlist, else 0 */
	uint8_t  instance;		 /* core 0 or 1 ... */
	uint64_t *function;		 /* pointer to the process function */
};

extern int process_attach(uint8_t instance, uint32_t period, void *function); // period is microseconds!
extern int process_detach(uint8_t instance, uint16_t pid);
extern int scheduler(uint8_t instance);

#define MILLISECONDS 1000
#define SECONDS 1000 * MILLISECONDS
#define MINUTES 60 * SECONDS

#define RUN(INSTANCE) \
	scheduler(INSTANCE)

#define PROCESS_ATTACH(INSTANCE, PERIOD, NAME) \
	process_attach(INSTANCE, PERIOD, NAME)

#define PROCESS_DETACH(INSTANCE, PID) \
	process_detach(INSTANCE, PID)

#endif // !SCHEDULER_H_