#include <string.h>
#include <stdio.h>

#include "scheduler.h"
#include "log.h"
#include "config.h"
#include "porting.h"

static struct pcb processlist[SCHEDULER_MAX_INSTANCE][SCHEDULER_MAX_PROCESSES];

int process_attach(uint8_t instance, uint32_t period, void *function)
{
	uint8_t i = 0;
	int ret = -1;

	if (instance > SCHEDULER_MAX_INSTANCE)
	{
		log_warn("attach process pid: %d at core %d is not valid!", i, instance);
		return ret;
	}

	while (i < SCHEDULER_MAX_PROCESSES)
	{
		if (processlist[instance][i].attached != 1)
		{
			log_debug("attach process pid: %d at core %d", i, instance);

			processlist[instance][i].instance = instance;
			processlist[instance][i].pid = i;
			processlist[instance][i].period = period;
			processlist[instance][i].function = function;
			processlist[instance][i].attached = 1;

			ret = 0;
			break;
		}
		
		i++;
	}

	return ret;
}

int process_detach(uint8_t instance, uint16_t pid)
{
	processlist[instance][pid].attached = 0;
	return 0;
}

int scheduler(uint8_t instance)
{
	void (*p)(uint8_t pid);

	while (1)
	{
		uint64_t now = getMicrosecTime();

		for (uint16_t i = 0; i < SCHEDULER_MAX_PROCESSES; i++)
		{
			if (processlist[instance][i].attached == 1)
			{
				if ( now >= processlist[instance][i].elapsed)
				{
					p = (void *)processlist[instance][i].function;
					(*p)(processlist[instance][i].pid);

					processlist[instance][i].elapsed = now + processlist[instance][i].period;
				}
			}
		}
		hardwareSchedulerRun();
	}

	return 0;
}