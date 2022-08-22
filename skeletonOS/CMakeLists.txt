add_library(skeletonOS INTERFACE)

set(LOG_USE_COLOR 1)

set(SCHEDULER_MAX_PROCESSES 32)
set(SCHEDULER_MAX_INSTANCE 2)
set(SKELETON_VERSION v1.0)

target_sources(skeletonOS INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/cbuf.c
    ${CMAKE_CURRENT_LIST_DIR}/src/json.c
    ${CMAKE_CURRENT_LIST_DIR}/src/log.c
    ${CMAKE_CURRENT_LIST_DIR}/src/scheduler.c
    ${CMAKE_CURRENT_LIST_DIR}/src/uuid4.c
    ${CMAKE_CURRENT_LIST_DIR}/src/dsp.c
    ${CMAKE_CURRENT_LIST_DIR}/src/fsm.c
    ${CMAKE_CURRENT_LIST_DIR}/porting.c
)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/config.h)

target_include_directories(skeletonOS INTERFACE 
    include
    ${CMAKE_CURRENT_BINARY_DIR}/generated/
)

target_link_libraries(skeletonOS INTERFACE
    m
)

