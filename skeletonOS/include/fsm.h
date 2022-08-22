#ifndef FSM_H_
#define FSM_H_

#include <stdio.h>
#include <stdint.h>

#define FSM_SIZE_MAX 32
#define EXCEPTION_JUMP 99
#define EXCEPTION_ZERO 69

typedef struct {
    uint8_t position;
    const char * name;
    void (*func)(void);
    uint8_t enable;
} stateFunctions;

typedef struct {
    uint8_t positionMAX;
    uint8_t position;
    uint8_t loopEN;
    stateFunctions stateFunctions[FSM_SIZE_MAX];
} FSM;

uint8_t FSM_Next(FSM *fsm);
const char * FSM_getStateName(FSM *fsm, uint8_t state);
void FSM_setPosition(FSM *fsm, uint8_t position);

#define FSM_NEXT(FSM) \
	FSM_Next(FSM)

#define FSM_GET_STATE(FSM, STATE) \
	FSM_getStateName(FSM, STATE)

#define FSM_GET_POSITION(FSM, POS) \
	FSM_setPosition(FSM, POS)

#endif // !FSM_H_