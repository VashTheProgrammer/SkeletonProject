#include <string.h>
#include <stdio.h>

#include "fsm.h"

uint8_t FSM_Next(FSM *fsm) 
{
    if (fsm->positionMAX == 0) 
        return EXCEPTION_ZERO;

    if ( (fsm->loopEN == 1) && (fsm->position == fsm->positionMAX) )
        fsm->position = 0;

    for(int i = 0; i < fsm->positionMAX; i++) 
    {
        if(fsm->stateFunctions[i].position == fsm->position)
        {
            fsm->position = fsm->position + 1;
            if(fsm->stateFunctions[i].enable == 1)
                (fsm->stateFunctions[i].func)();
            else 
                return EXCEPTION_JUMP;

            return fsm->position - 1;
        }
    }

    return fsm->position - 1;
}

const char * FSM_getStateName(FSM *fsm, uint8_t state) {

    if (state == EXCEPTION_JUMP)
        return "JUMP";

    return fsm->stateFunctions[state].name;
}

void FSM_setPosition(FSM *fsm, uint8_t position) {
    fsm->position = position;
}