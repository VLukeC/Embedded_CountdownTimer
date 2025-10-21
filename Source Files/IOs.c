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
uint8_t min=0, sec=0;
extern uint8_t T2flag;
extern uint8_t running;
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



void IOcheck(){
    uint8_t pressed = pb1 + pb2 + pb3;
    // If not running
    if(!running){ 

        if(pressed == 0){
        }
        else if(pressed == 1){
            if(pb1){
                if(pressDuration >= 6 && sec % 5 == 0){
                    sec+=5;
                }
                else{
                    sec+=1;
                }
                if(sec>59){sec=0;}
                DispTime(min,sec);


            }
            else if(pb2){
                min+=1;
                if(min>59){min=0;}
                DispTime(min,sec);

            }
        }
        else if(pressed == 2){
            // Check PB1 & PB2 and determine if it is long or short press
            if(pb1 && pb2){
                if(!longPress){
                    //Start countdown
                    running = 1;
                    startTimer(min, sec);
                    

                }
                if(longPress){
                    //Reset time to 00:00
                    min = 0;
                    sec = 0;
                    DispTime(min,sec);
                }

            }
            else if(pb2 && pb3){
                Disp2String("\rPB2 and PB3 are pressed");
            }
            else{
                DispTime(min,sec);
            }
        }
        else if(pressed == 3){
            LATBbits.LATB9 = 1;
            Disp2String("\r2025 ENSF 460 L02 - Group02");
        }
    }
    // If timer running
    if(running){
        
    }
}




