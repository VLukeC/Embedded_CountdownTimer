#ifndef IOs_H
#define IOs_H

#include <xc.h>
extern uint8_t pb1, pb2, pb3;
extern uint8_t min,sec;
extern volatile uint8_t longPress, pressDuration, pressActive, pressDone;
extern uint8_t running;
// Function declarations
void IOinit();

void IOcheck();

#endif