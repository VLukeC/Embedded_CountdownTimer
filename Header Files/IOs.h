#ifndef IOs_H
#define Ios_H

#include <xc.h>
extern volatile uint8_t longPress;     
extern volatile uint8_t pressActive;
extern uint8_t min, sec;
extern uint8_t paused;
extern uint8_t pb1Event, pb2Event, pb3Event;

// Function declarations
void IOinit();

void increment_delay(void);

void IOcheck();

#endif