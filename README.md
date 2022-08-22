# SkeletonOS

## Idea

things to do while contributing to the project: 
- watch Ronnie O'Sullivan match (before).
- listen to Apparat.
- eat ice cream (only for summer season).

My idea is to put together the basic tools to create a skeleton project in C. Through skeleton the user can be immediately operational, he will have:
- Scheduler
- Logger
- Circular buffer
- JSON config
- lightDSP
- CMake like
- Multi architecture support (porting ready)

Time permitting, I'll add more ...
MD Syntax Guide: https://www.markdownguide.org/basic-syntax/ 

## MacOS

### Requirements

#### Toolchain
(https://andrejacobs.org/electronics/develop-and-debug-raspberry-pi-pico-on-macos-big-sur/)

1. Ensure everything is ok and update homebrew packages.
```
$ brew doctor
$ brew update
$ brew upgrade
```
2. Install the Toolchain (following section 9.1 Building on Apple macOS from the Official guide)
```
$ brew install cmake
$ brew tap ArmMbed/homebrew-formulae
$ brew install arm-none-eabi-gcc
```

### Compile and Run (MacOS)
On your Terminal: 
```
$ git clone https://github.com/VashTheProgrammer/SkeletonProject/
```
go on VSC, open Skeleton folder, setup the project with GCC arm toolchain and ...
that's it!

## Linux

### Requirements
```
$ apt install texinfo build-essential manpages-dev make
```

### Compile and Run (Linux)
On your Terminal:
```
git clone https://github.com/VashTheProgrammer/SkeletonProject/
cd SkeletonProject/helloWorld
mkdir build
cd build
cmake ..
make
./helloWorld
```

## How does it work ?

### Conventions
Function returns (int):
 - (0)   -> Success
 - (-1)  -> Error
<br/><br/>


### HelloWorld (helloWorld.c)

** main.c **

It can be used to test all skeleton components.
Inside you will find 2 tasks with two different periods, one pushes and the other pops on a circular test buffer and it is possible to debug through the logger.
The comments explain how the library APIs work.
All API is masked by DEFINE Macro.
<br/><br/>

### Scheduler

**lib: scheduler.c and include/scheduler.h**

API:
```c
// name task, period is milliseconds, function call - return pid
int PROCESS_ATTACH(char *name, uint32_t period, void *function); 

// pid of the task - return successful or not
int PROCESS_DETACH(uint16_t pid);

// return successful or not - contains while(1)
int RUN();
```

Tricks: 
```c
usleep(MIN_TIMESLOT_NS); // 1 millisecond for CPU relax into "scheduler()"
```
<br/>

### Logger

**lib: logger.c and include/logger.h**

API:
```c
#define LOGGER_LEVEL_TRACE          LOW
#define LOGGER_LEVEL_DEBUG          ...
#define LOGGER_LEVEL_INFO           ...
#define LOGGER_LEVEL_WARNING        ...
#define LOGGER_LEVEL_ERROR          ...
#define LOGGER_LEVEL_FATAL          HIGH

// file name and path, logger level start, stdout enable/disable - return successful or not, [MUST] put on the main() head 
int LOGGER_INIT("test.log", LOGGER_LEVEL_DEBUG, LOGGER_STDOUT_ON);

// using like classic printf - return successful or not
int LOGGER_ERROR("color error\n");
int LOGGER_WARN("color warning\n");
int LOGGER_INFO("color infomation\n");
int LOGGER_DEBUG("color debug\n");

// close file, auto flush is always present for every logger raw - return successful or not
int LOGGER_CLOSE();
```
<br/>

### Circular Buffer
**lib: cbuf.c and include/cbuf.h**

API:
```c
// Defines a global circular buffer `buf` of a given type and size. The type
// can be native data types or user-defined data types.
// Usage:
// CIRC_GBUF_DEF(uint8_t, byte_buf, 13);
// CIRC_GBUF_DEF(struct foo, foo_buf, 10);
int CIRC_GBUF_DEF(type, buf, size);

// Resets the circular buffer offsets to zero. Does not clean the newly freed slots. 
int CIRC_GBUF_FLUSH(buf);

// Pushes element pointed to by `elem` at the head of circular buffer `buf`.
// This is read-write method, occupancy count increases by one.
int CIRC_GBUF_PUSH(buf, elem)  _push_refd(elem);

// Copies the element at tail of circular buffer `buf` into location pointed
// by `elem`. This method is read-only, does not later occupancy status. 
int CIRC_GBUF_POP(buf, elem)  _pop_refd(elem);

// Returns the number of free slots in the circular buffer `buf` - return: number of slots available.
int CIRC_GBUF_FS(buf)   __circ_gbuf_free_space(&buf);

// OPTIONS:
// Zero slots in circular buffer after a pop.
int CRICBUF_CLEAN_ON_POP
```
<br/>

```
<br/>

## TIPS

#### Visual Studio Code
CMD+SHIFT+V -> .MD Preview on/off
Probably need to install something

# Have fun!
REM said, remember, the ticket to the future is always blank.<br/>
Vash
