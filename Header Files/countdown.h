#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <xc.h>

extern volatile uint16_t countdown;


void startTimer(uint8_t min, uint8_t sec);
void updateTimer(void);
void alarm_flash(void);

#endif