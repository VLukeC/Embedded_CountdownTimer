/*
 * File:   IOs.c
 * Author: Landon
 *
 * Created on September 26, 2025, 3:16 PM
 */

#include "IOs.h"


uint16_t downcount;
extern uint8_t pb1, pb2, pb3;
extern uint8_t min=0,sec=0;
extern uint16_t T2flag;
extern bool running = false;
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



void IOcheck(){
    uint8_t pressed = pb1 + pb2 + pb3;
    if(pressed == 0){
        LATBbits.LATB9 = 0;
    }
    else if(pressed == 1){
        if(pb1){
            LATBbits.LATB9 = 1;
            sec+=1;
            if(sec>59){sec=0;}
            DispTime(min,sec);
            
            
        }
        else if(pb2){
            LATBbits.LATB9 = 1;
            min+=1;
            if(min>59){min=0;}
            DispTime(min,sec);
                      
        }
        else{
            LATBbits.LATB9 = 1;
           
            Disp2String("\rPB3 pressed");            
        }
    }
    else if(pressed == 2){
        LATBbits.LATB9 = 1;
        if(pb1 && pb2){
            Disp2String("\rPB1 and PB2 are pressed");
            T3CONbits.TON=0;
            T3CONbits.TCKPS=1;
            PR3=((min*60)+sec)/0.000256f ;//needs to be adjusted based on the clock 
            downcount = PR3 - TMR2;

        }
        else if(pb2 && pb3){
            Disp2String("\rPB2 and PB3 are pressed");
        }
        else{
            Disp2String("\rPB1 and PB3 are pressed");
        }
    }
    else if(pressed == 3){
        LATBbits.LATB9 = 1;
        Disp2String("\r2025 ENSF 460 L02 ? Group02");
    }
    
}


