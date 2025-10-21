/*
 * File:   IOs.c
 * Author: Landon
 *
 * Created on September 26, 2025, 3:16 PM
 */

#include "IOs.h"
#include "UART2.h"
#include "countdown.h"


extern uint8_t pb1, pb2, pb3;
extern uint8_t min=0, sec=0;
extern uint8_t T2flag;
extern uint8_t running;
extern uint8_t paused;
extern uint8_t ioFlag;
extern uint8_t pb1Event, pb2Event, pb3Event;
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
    
    // UArt
    
   
}



void IOcheck(void) {
    // Only one press type is handled at a time for simplicity
    if (!running) {
        //PB 1 Press
        if (pb1Event && !pb2Event && !pb3Event) {
            // PB1 short or long hold
            if (longPress) {
                sec += 5;
            } else {
                sec += 1;
            }
            if (sec > 59) sec = 0;
            DispTime(min, sec);
        }
        // PB 2 pressed
        else if (pb2Event && !pb1Event && !pb3Event) {
            // PB2 short press
            min += 1;
            if (min > 59) min = 0;
            DispTime(min, sec);
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
            if (!longPress) {
                running = 1;
                startTimer(min, sec);
            } else {
                min = 0;
                sec = 0;
                DispTime(min, sec);
            }
        }
        //PB2 and PB3 pressed
        else if (pb2Event && pb3Event && !pb1Event) {
            Disp2String("\rPB2 and PB3 pressed");
        }
        //PB1 and PB3 pressed
        else if (pb1Event && pb3Event && !pb2Event) {
            Disp2String("\rPB1 and PB3 pressed");
        }
        else if(pb1Event && pb2Event && pb3Event){
            LATBbits.LATB9 = 1;
            Disp2String("\r2025 ENSF 460 L02 - Group02");
        }

        // clear all events after handling
        pb1Event = pb2Event = pb3Event = 0;
    }
    if(running){
        if(pb3Event && !pb1Event && !pb2Event){
            T2CONbits.TON = 0; 
            running = 0;
            paused = 1;
        }
        pb1Event = pb2Event = pb3Event = 0;
    }
    
}



