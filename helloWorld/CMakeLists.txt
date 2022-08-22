cmake_minimum_required(VERSION 3.13)

project(skeletonExample C CXX)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
    -Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
)

add_subdirectory(../skeletonOS build)

add_executable(skeletonExample
    helloWorld.c
)

target_link_libraries(skeletonExample 
    skeletonOS
)
