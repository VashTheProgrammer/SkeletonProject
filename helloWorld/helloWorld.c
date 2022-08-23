#include "skeletonOS.h"

#define SCHEDULER_INSTANCE 0
#define TASK1_PERIOD 1 * 1000 * 1000
#define TASK2_PERIOD 1 * 1000 * 1000 * 10

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

int main()
{
    log_info("Skeleton %s starting...\n", SKELETON_VERSION);

    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK1_PERIOD, test);
    PROCESS_ATTACH(SCHEDULER_INSTANCE, TASK2_PERIOD, logger_test);
    
    RUN(SCHEDULER_INSTANCE);

    return 0;
}