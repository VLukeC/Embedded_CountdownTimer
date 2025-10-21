/*
 * File:   updateTimer.c
 * Author: Luke Chayer
 *
 * Created on October 20, 2025, 12:38 PM
 */


#include "xc.h"
#include "countdown.h"
#include "UART2.h"
volatile uint8_t countdown = 0;
extern uint8_t T2flag;
extern uint8_t running;

void startTimer(uint8_t min, uint8_t sec){
    DispTime(min,sec);
    running = 1;
    countdown = min*60 + sec;
    LATBbits.LATB9 = !LATBbits.LATB9; // switch for LED
}

void updateTimer() {
    T2CONbits.TON = 1;
    while(!T2flag){}
    LATBbits.LATB9 = !LATBbits.LATB9; // switch for LED
    countdown -= 1;
    uint8_t mins = countdown/60;
    uint8_t secs = countdown % 60;
    DispTime(mins,secs);
    if(countdown == 0){
        running = 0;
    }
    
}
