/*
 * File:   IOs.c
 * Author: Landon Reed, Luke Chayer, Sam Shojaei
 *
 * Created on October 21, 2025, 8:10 PM
 */

#include "IOs.h"
#include "UART2.h"
#include "countdown.h"

uint8_t clr;
extern uint8_t pb1, pb2, pb3;
extern uint8_t min=0, sec=0;
extern uint8_t T1flag;
extern uint8_t T2flag;
extern uint8_t T3flag;
extern uint8_t running;
extern uint8_t paused;
extern uint8_t pb1Event, pb2Event, pb3Event;
extern volatile uint8_t longPress;
void IOinit() {
    // LEDS
    TRISBbits.TRISB9 = 0;
    LATBbits.LATB9 = 0;

    TRISAbits.TRISA6 = 0;
    LATAbits.LATA6 = 0;
    
    // CN Interrupt config
    
    IPC4bits.CNIP = 6;     
    IFS1bits.CNIF = 0;
    
    // PB setup
    
    TRISBbits.TRISB7 = 1;
    CNPU2bits.CN23PUE = 1;
    CNEN2bits.CN23IE = 1;  
    
            
    TRISBbits.TRISB4 = 1;
    CNPU1bits.CN1PUE = 1;
    CNEN1bits.CN1IE = 1;
    
    TRISAbits.TRISA4 = 1;
    CNPU1bits.CN0PUE = 1;
    CNEN1bits.CN0IE = 1;
    
    IEC1bits.CNIE = 1;
    
    
}

//Delays incrementing of value while button held.
void increment_delay(void){
    T1flag = 0;
    TMR1 = 0;
    T1CONbits.TON = 1;
}

void register_inputs(void){
    if(T3flag){
    //Check inputs:
        T3flag = 0;
        pb1 = !PORTBbits.RB7;
        pb2 = !PORTBbits.RB4;
        pb3 = !PORTAbits.RA4;
        uint8_t anyPressed = pb1 || pb2 || pb3;
        if (anyPressed) {
            if (pb1) pb1Event = 1;
            if (pb2) pb2Event = 1;
            if (pb3) pb3Event = 1;
            longPress = 0;
            pressActive = 1;

            // Increment_delay
            TMR1 =0;
            T1CONbits.TON = 1;
            T1flag = 1; //for initial presses
        }
        else if (pressActive) {
            longPress = 0;
            pressActive = 0;
            T3CONbits.TON = 0;
            T1CONbits.TON = 0;
        }
        IOcheck();
    }
}


void IOcheck(void) {
    // Only one press type is handled at a time for simplicity
    if (!running) {
        //PB 1 Press
        if (pb1Event && !pb2Event && !pb3Event) {
            while(pressActive && !pb2Event){// PB1 short or long hold
                if (longPress && T1flag ==1) {
                    increment_delay();
                    sec += 5;
                } else if(T1flag == 1){
                    increment_delay();
                    sec += 1;
                }
                if (sec > 59) sec = 0;
                DispTime(min, sec);
            }
        }
        // PB 2 pressed
        else if (pb2Event && !pb1Event && !pb3Event) {
            // PB2 short press
            while(pressActive && !pb1Event){
                if(T1flag == 1){
                    increment_delay();
                    min+=1;
                }
                if (min > 59) min = 0;
                DispTime(min, sec);
            }
            
        }
        //PB 3 pressed
        else if(pb3Event && !pb1Event && !pb2Event){
            if(paused){
                running = 1;
                T2CONbits.TON = 1; 
            }
        }
        // PB1 and PB2 pressed
        else if (pb1Event && pb2Event && !pb3Event) {
            while(pressActive && !longPress){} //Wait to see if long press is triggered while buttons held
            if (!longPress) {
                running = 1;
                startTimer(min, sec);
            } else {
                min = 0;
                sec = 0;
                DispTime(min, sec);
            }
        }
        // PB3 Held and PB2 Pressed
        else if (pb2Event && pb3Event && !pb1Event) {
            while(pressActive){
                if(pb2Event){
                    pb2Event = 0;
                    min--;
                    if(min == 255){
                        min = 59;
                    }
                    DispTime(min, sec);
                }
            }
        }
        // PB3 Held and PB1 Pressed
        else if (pb1Event && pb3Event && !pb2Event) {
            while(pressActive){
                if(pb1Event){
                    pb1Event = 0;
                    sec--;
                    if(sec == 255){
                        sec = 59;
                    }
                    DispTime(min, sec);
                }
            }
        }
        // All Three held
        else if(pb1Event && pb2Event && pb3Event){
            while(pressActive){
                Disp2String("\r2025 ENSF 460 L02 - Group 02");
            }
            //Clear line and display timer again
            Disp2String("\r                                  ");
            DispTime(min,sec);
        }

        // clear all events after handling
        pb1Event = pb2Event = pb3Event = 0;
    }
    if(running){
        if(pb3Event && !pb1Event && !pb2Event){
            while(pressActive && !longPress){} // Wait to see if long press while held
            if (!longPress){
                T2CONbits.TON = 0; 
                running = 0;
                paused = 1;
            }
            else {
                running = 0;
                clr = 1;
                min = 0;
                sec = 0;
                DispTime(min, sec);
            }
        }
        pb1Event = pb2Event = pb3Event = 0;
    }
    
}



