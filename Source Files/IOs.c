/*
 * File:   IOs.c
 * Author: Landon Reed, Luke Chayer, Sam Shojaei
 *
 * Created on October 21, 2025, 8:10 PM
 */

#include "IOs.h"
#include "UART2.h"
#include "countdown.h"

// Variables
extern uint8_t pb1, pb2, pb3;   // Button states
extern uint8_t min=0, sec=0;    // Timer minutes and seconds
extern uint8_t T1flag;          // Timer 1 flag
extern uint8_t T2flag;          // Timer 2 flag
extern uint8_t running;         // Flag for if the program is running
extern uint8_t paused;          // Flag for if the program is paused
extern uint8_t pb1Event, pb2Event, pb3Event;    // Flag for if a button press has occured
extern volatile uint8_t longPress;          // Flag for if the button is held for long period

// Initilizes all the Led, pbs and CN interrupts
void IOinit() {
    // LEDS
    TRISBbits.TRISB9 = 0;   //RB9 output
    LATBbits.LATB9 = 0;     // LED starts off

    TRISAbits.TRISA6 = 0;   //RA6 output
    LATAbits.LATA6 = 0;     //LED starts off
    
    // CN Interrupt config
    
    IPC4bits.CNIP = 6;     // CN interrupt priority level
    IFS1bits.CNIF = 0;     // Clear CN flag
    
    // PB setup
    
    // RB7 setup    
    TRISBbits.TRISB7 = 1;      // RB7 as input
    CNPU2bits.CN23PUE = 1;      //Enable pullup
    CNEN2bits.CN23IE = 1;       // Enable CN interrupt
    
    // RB4 setup
    TRISBbits.TRISB4 = 1;   // RB4 as input
    CNPU1bits.CN1PUE = 1;   // Enable pullup
    CNEN1bits.CN1IE = 1;    // Enable CN interrupt
    
    // RA4 setup
    TRISAbits.TRISA4 = 1;   // RA4 as input
    CNPU1bits.CN0PUE = 1;   // Enable pullup
    CNEN1bits.CN0IE = 1;    // Enable CN interrupt
    
    // Enable system wide CN interrupts
    IEC1bits.CNIE = 1;
    
    
}

//Delays incrementing of value while button held.
void increment_delay(void){
    T1flag = 0;     // Clear flag
    TMR1 = 0;       // Reset timer 1 counter
    T1CONbits.TON = 1;  // Turn timer 1 back on
}


// Checks which button is pressed and handles the 
void IOcheck(void) {
    // Only one press type is handled at a time for simplicity
    // When the timer is not running
    if (!running) {
        //PB 1 Press
        if (pb1Event && !pb2Event && !pb3Event) {
            while(pressActive && !pb2Event){// PB1 short or long hold
                // When there is a longhold
                if (longPress && T1flag ==1) {
                    increment_delay();
                    sec += 5;
                //Short press
                } else if(T1flag == 1){
                    increment_delay();
                    sec += 1;
                }
                // If sec becomes 60 make sec = 0
                if (sec > 59) sec = 0;
                DispTime(min, sec); // Update display
            }
        }
        // PB 2 pressed
        else if (pb2Event && !pb1Event && !pb3Event) {
            // PB2 short press
            while(pressActive && !pb1Event){
                if(T1flag == 1){
                    // Add 1 minute per interval
                    increment_delay();
                    min+=1;
                }
                if (min > 59) min = 0; //Wrap around at 60 minutes
                DispTime(min, sec); // Display time
            }
            
        }
        //PB 3 pressed
        else if(pb3Event && !pb1Event && !pb2Event){
            if(paused){
                // Short press -> start timer
                running = 1;
                T2CONbits.TON = 1;  // Resume Timer 2
            }
        }
        // PB1 and PB2 pressed
        else if (pb1Event && pb2Event && !pb3Event) {
            while(pressActive && !longPress){} //Wait to see if long press is triggered while buttons held
            // If no long press while holding buttons
            if (!longPress) {
                running = 1;
                startTimer(min, sec);
            } 
            // IF there is a long press rest timer values
            else {
                min = 0;
                sec = 0;
                DispTime(min, sec);
            }
        }
        // PB3 Held and PB2 Pressed
        else if (pb2Event && pb3Event && !pb1Event) {
            while(pressActive){
                if(pb2Event){
                    // Clears pb2 flag
                    pb2Event = 0;
                    // Decrements minuts
                    min--;
                    if(min == 255){
                        min = 59;
                    }
                    DispTime(min, sec); // Displays updated time
                }
            }
        }
        // PB3 Held and PB1 Pressed
        else if (pb1Event && pb3Event && !pb2Event) {
            while(pressActive){
                if(pb1Event){
                    // Clears pb1 flag
                    pb1Event = 0;
                    // Decrements seconds
                    sec--;
                    if(sec == 255){
                        sec = 59;
                    }
                    DispTime(min, sec); // Displats updated time
                }
            }
        }
        // All Three held
        else if(pb1Event && pb2Event && pb3Event){
            while(pressActive){
                // Display group info
                Disp2String("\r2025 ENSF 460 L02 - Group 02");
            }
            //Clear line and display timer again
            Disp2String("\r                                  ");
            DispTime(min,sec);
        }

        // clear all events after handling
        pb1Event = pb2Event = pb3Event = 0;
    }
    // When the timer is running
    if(running){
        // PB3
        if(pb3Event && !pb1Event && !pb2Event){
            while(pressActive && !longPress){} // Wait to see if long press while held
            if (!longPress){
                // If there is a short press, just pause the countdown
                T2CONbits.TON = 0; 
                running = 0;
                paused = 1;
            }
            // If there is a longpause reset the timer and make sure it is not running
            else {
                running = 0;
                min = 0;
                sec = 0;
                DispTime(min, sec);
            }
        }
        pb1Event = pb2Event = pb3Event = 0; // Reset the pb event flags
    }
    
}



