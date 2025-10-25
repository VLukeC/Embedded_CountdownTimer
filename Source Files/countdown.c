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
extern uint8_t alarm;       // Flag indicating that the alarm is active
volatile uint8_t countdown = 0; // Total remaining countdown time in seconds
extern uint8_t T2flag;      // Set by Timer 2 ISR every 1 second
extern uint8_t running; // Indicates if the countdown is running
extern uint8_t T1flag;  // Flag for T1Interrupt

/**
Starts the timer
@param min Minutes to count down from
@param sec Seconds to count down from
*/
void startTimer(uint8_t min, uint8_t sec){
    // Display initial time
    DispTime(min,sec);
    // Set running flag to 1 to start the timer
    running = 1;
    // count down is equal to total time in seconds
    countdown = min*60 + sec;
    LATBbits.LATB9 = !LATBbits.LATB9; // switch for LED
}


// Updates the timer as the countdown continues
void updateTimer(void) {
    // Start timer 2 if it is not already running
    if (!T2CONbits.TON) {
        TMR2 = 0;   // Reset timer 2 counter
        IFS0bits.T2IF = 0;  //Clear interrupt flag
        T2flag = 0;     // Clear t2flag
        T2CONbits.TON = 1;  // Enable timer 2
    }

    // T2 flag is set every 1 second from T2 interrupt
    if (T2flag) {
        T2flag = 0;     //Reset flag

        LATBbits.LATB9 ^= 1; //Toggle Led every second

        // If the countdown is greater then 0 decrement
        if (countdown > 0) {
            countdown--;
            min = countdown / 60;   // Calculate new mins
            sec = countdown % 60;   // Calculate new secs
            DispTime(min, sec);
        }

        // Once countdown reaches 0
        if (countdown == 0) {
            alarm = 1;  // Activate alarm
            running = 0;    // Stop the program from running
            T2CONbits.TON = 0;  // Stop timer 2
            Disp2String("\rFIN 00m : 00s - ALARM"); // Display alarm message
        }
    }
}


// Controls the LED flashing while alarm is active
void alarm_flash(void){
    // Set LED1 ON and flash LED2 rapidly
    LATBbits.LATB9 = 1;
    if(T1flag){
        T1flag = 0;     //Reset flag
        TMR1 = 0;       //Reset count for t1
        T1CONbits.TON = 1;  // Restart T1
        LATAbits.LATA6 = !LATAbits.LATA6; //Flash LED2
    }
}
