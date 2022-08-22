#include "skeletonOS.h"

int test(uint8_t pid)
{
    printf("test\n");
    return 0;
}

int main()
{

    log_info("Skeleton %s starting...\n", SKELETON_VERSION);

    PROCESS_ATTACH(0, 1 * 1000 * 1000, test);

    RUN(0);

    return 0;
}