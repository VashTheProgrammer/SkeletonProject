#include "skeletonOS.h"

#define SCHEDULER_INSTANCE 0
#define TASK1_PERIOD 1 * 1000 * 1000
#define TASK2_PERIOD 1 * 1000 * 1000 * 10

#define TASK_PUSH_PERIOD 5 * 1000 * 1000
#define TASK_POP_PERIOD 1 * 1000 * 1000

#define CBUF_DATA_SIZE 1024

struct Element
{
	int value;
	uint64_t timestamp;
};

circ_gbuf_t cBufData;
CIRC_GBUF_DEF(struct Element, cBufData, CBUF_DATA_SIZE);

int test(uint8_t pid)
{
    printf("test\n");

    return 0;
}

int logger_test(uint8_t pid)
{
    log_trace("logger trace test");
    log_debug("logger debug test");
    log_info("logger info test");
    log_warn("logger warning test");
    log_error("logger error test");
    log_fatal("logger fatal test");

    return 0;
}

int cBuf_Push(uint8_t pid)
{
    struct Element element;

    for(int i = 0; i < 5; i++)
    {
        element.timestamp = getMillisTime();
        element.value = rnd();

        int slotFull = CBUF_DATA_SIZE - CIRC_GBUF_FS(cBufData);
	    log_trace("cBuf slot full: %d", slotFull);

        if (CIRC_GBUF_PUSH(cBufData, &element) == 0)
	    	{
	    		printf("Push: %d at: %u\n", element.value, element.timestamp);
	    	}
    }
    
    return 0;
}

int cBuf_Pop(uint8_t pid)
{
    struct Element element;

    if (CIRC_GBUF_POP(cBufData, &element) == 0)
		{
            printf("Pop: %d at: %u\n", element.value, element.timestamp);
        }

        return 0;
}

int main()
{
    log_info("Skeleton %s starting...\n", SKELETON_VERSION);

    CIRC_GBUF_FLUSH(cBufData);

    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK1_PERIOD, test);
    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK2_PERIOD, logger_test);
    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK_PUSH_PERIOD, cBuf_Push);
    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK_POP_PERIOD, cBuf_Pop);
    
    RUN(SCHEDULER_INSTANCE);

    return 0;
}