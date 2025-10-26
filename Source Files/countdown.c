/*
 * File:   updateTimer.c
 * Author: Luke Chayer
 *
 * Created on October 20, 2025, 12:38 PM
 */


 // Includes
#include "xc.h"
#include "countdown.h"
#include "UART2.h"
#include "IOs.h"

// Variable
extern uint8_t alarm;   // Flag for alarm
volatile uint16_t countdown = 0;    // Countdown timer in seconds
extern uint8_t T2flag;          // T2 event
extern uint8_t running;         /// Running timer flag
extern uint8_t T1flag;          // T1 event

// Starts the countdown timer
// Takes minutes and seconds as arguments and returns void
void startTimer(uint8_t min, uint8_t sec){
    DispTime(min,sec);      // Display initial time
    running = 1;            // Set running flag
    countdown = (min*60) + sec; // Set countdown in seconds
    LATBbits.LATB9 = !LATBbits.LATB9; // switch for LED
}


// Updates the timer as the clock runs
void updateTimer(void) {
    // Make sure T2 is running
    if (!T2CONbits.TON) {
        TMR2 = 0;
        IFS0bits.T2IF = 0;
        T2flag = 0;
        T2CONbits.TON = 1;
    }
    // Every 1 second this flag will be true
    if (T2flag) {
        T2flag = 0;

        LATBbits.LATB9 ^= 1; // Switch LED each second

        // Decrement countdown
        if (countdown > 0) {
            countdown--;
            min = countdown / 60;
            sec = countdown % 60;
            DispTime(min, sec);
        }

        // If countdown reaches 0 raise alarm and stop running
        if (countdown == 0) {
            alarm = 1;
            running = 0;
            T2CONbits.TON = 0; 
            Disp2String("\rFIN 00m : 00s - ALARM");
        }
    }
}


// Make the alarm flash
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
