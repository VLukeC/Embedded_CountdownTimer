/*
 * File:   TimeDelay.c
 * Author: Luke Chayer
 *
 * Created on September 26, 2025, 2:34 PM
 */


#include "xc.h"
#include "TimeDelay.h"

void delay_ms(uint16_t time_ms) {
    float pr2_val = (float)time_ms/0.256f;
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 2;
    T2CONbits.TCS = 0;
    T2CONbits.TSIDL = 0;
    
    IPC1bits.T2IP0 = 7;
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;
    

    PR2= pr2_val;
    TMR2 = 0;
    
    T2CONbits.TON = 1;
    
    return;
}
