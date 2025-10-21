#ifndef IOs_H
#define Ios_H

#include <xc.h>
extern volatile uint8_t longPress;     
extern volatile uint8_t pressDuration;
extern volatile uint8_t pressActive;
extern volatile uint8_t pressDone;
extern uint8_t min, sec;
extern uint8_t paused;
extern uint8_t ioFlag;
extern uint8_t pb1Event, pb2Event, pb3Event;

// Function declarations
void IOinit();

void IOcheck();

#endif