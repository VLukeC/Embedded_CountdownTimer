/*
 * File:   updateTimer.c
 * Author: Luke Chayer
 *
 * Created on October 20, 2025, 12:38 PM
 */


#include "xc.h"
#include "countdown.h"
#include "UART2.h"
#include "IOs.h"
extern uint8_t alarm;
volatile uint8_t countdown = 0;
extern uint8_t T2flag;
extern uint8_t running;
extern uint8_t T1flag;

void startTimer(uint8_t min, uint8_t sec){
    DispTime(min,sec);
    running = 1;
    countdown = min*60 + sec;
    LATBbits.LATB9 = !LATBbits.LATB9; // switch for LED
}

void updateTimer(void) {
    if (!T2CONbits.TON) {
        TMR2 = 0;
        IFS0bits.T2IF = 0;
        T2flag = 0;
        T2CONbits.TON = 1;
    }

    if (T2flag) {
        T2flag = 0;

        LATBbits.LATB9 ^= 1; 

        if (countdown > 0) {
            countdown--;
            min = countdown / 60;
            sec = countdown % 60;
            DispTime(min, sec);
        }

        if (countdown == 0) {
            alarm = 1;
            running = 0;
            T2CONbits.TON = 0; 
            Disp2String("\rFIN 00m : 00s - ALARM");
        }
    }
}

void alarm_flash(void){
    // Set LED1 ON and flash LED2 rapidly
    LATBbits.LATB9 = 1;
    if(T1flag){
        T1flag = 0;
        TMR1 = 0;
        T1CONbits.TON = 1;
        LATAbits.LATA6 = !LATAbits.LATA6; //Flash LED2
    }
}
