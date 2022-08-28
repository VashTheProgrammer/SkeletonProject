#include "skeletonOS.h"

#define SCHEDULER_INSTANCE 0
#define TASK1_PERIOD 1 * 1000 * 1000
#define TASK2_PERIOD 1 * 1000 * 1000 * 10

#define CBUF_HFDATA_SIZE 8192

struct Element
{
	int value;
	uint64_t timestamp;
};



circ_gbuf_t cBufHFData;
CIRC_GBUF_DEF(struct Element, cBufHFData, CBUF_HFDATA_SIZE);



int test(uint8_t pid)
{
    printf("test\n");

    struct Element element;

    element.timestamp = getMicrosecTime();

    element.value = rand();

    if (CIRC_GBUF_PUSH(cBufHFData, &element) == 0)
		{
			printf("Push: %d at: %d\n", element.value, element.timestamp);
		}

    return 0;
}

int logger_test(uint8_t pid)
{
    struct Element element;

    element.timestamp = 69;

    element.value = 69;

    printf("Pop out: %d at: %d\n", element.value, element.timestamp);

    if (CIRC_GBUF_POP(cBufHFData, &element) == 0)
		{
            printf("Pop in: %d at: %d\n", element.value, element.timestamp);
        }

    
    
    log_trace("logger trace test");
    log_debug("logger debug test");
    log_info("logger info test");
    log_warn("logger warning test");
    log_error("logger error test");
    log_fatal("logger fatal test");

    return 0;
}

int main()
{
    log_info("Skeleton %s starting...\n", SKELETON_VERSION);

    CIRC_GBUF_FLUSH(cBufHFData);

    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK1_PERIOD, test);
    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK2_PERIOD, logger_test);
    
    RUN(SCHEDULER_INSTANCE);

    return 0;
}