/*
 * File:   main.c
 * Author: Landon Reed
 *
 * Created on: USE THE INFORMATION FROM THE HEADER MPLAB X IDE GENERATES FOR YOU
 */

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC oscillator (FRC))
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON            // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (PGC2/PGD2 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC            // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)

// #pragma config statements should precede project file includes.

#include <xc.h>
#include <p24F16KA101.h>
#include "clkChange.h"
#include "UART2.h"
#include "IOs.h"
uint8_t CN_flag = 0;
uint16_t PB_event = 0;
extern uint8_t longPress = 0; // Flag for the long press
extern uint8_t pressDuration = 0; // Every 0.5s this is incremented so when it reaches 6 it is a 3s long press
extern uint8_t pressActive = 0; //Checks if any button is pressed




/**
 * You might find it useful to add your own #defines to improve readability here
 */

int main(void) {
    AD1PCFG = 0xFFFF; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    newClk(500);
    // === TIMER 1: CN-DEBOUNCE TIMER ===
    T1CONbits.TON = 0;      // off until CN fires
    T1CONbits.TCS = 0;      // internal clock (Fcy)
    T1CONbits.TCKPS = 1;      // prescaler 1:8
    T1CONbits.TSIDL = 0;      // continue in Idle
    IPC0bits.T1IP = 3;      // higher priority than T3
    IFS0bits.T1IF = 0;      // clear interrupt flag
    IEC0bits.T1IE = 1;      // enable interrupt
    PR1 = 6250;   // ≈10 ms @ 500 kHz Fcy, prescaler 1:8
    TMR1 = 0;


    // === TIMER 3: Press-duration timer ===
    T3CONbits.TON = 0;      // keep off until needed
    T3CONbits.TCS = 0;      // internal clock (Fcy)
    T3CONbits.TCKPS = 3;      // prescaler 1:256
    T3CONbits.TSIDL = 0;      // run in idle mode
    IPC2bits.T3IP = 2;      // interrupt priority = 2
    IFS0bits.T3IF = 0;      // clear flag
    IEC0bits.T3IE = 1;      // enable interrupt
    PR3 = 5859;   // ≈0.5 s period at Fcy = 500 kHz, 1:256 prescale
    TMR3 = 0;
    
    

   
    /* Initilizations */    
    IOinit();
  
    
    while(1) {
        
        Idle();
        // If a PB event occurs run this code
        if (PB_event) {
            IOcheck();
            PB_event = 0;
        }
    }
    
    return 0;
}

// Timer 1 interrupt bounce handeling
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;    
    T1CONbits.TON = 0;
    // Check if the 10ms debounce timer has gone off     
    if (CN_flag) {
        CN_flag = 0;
        PB_event = 1;      
    }
}


// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {

}

// Timer 3 interrupt Long vs Short button press
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    IFS0bits.T3IF = 0;
    //Checks if a button is still pressed
    if(pressActive){
        // If a button is still pressed increment pressDuration
        pressDuration++;
        if(pressDuration >= 6){
            longPress = 1;
        }
    }
    //Todo: make sure in IOCheck when button is not pressed it stops timer 3
    //Todo: make sure in IOcheck when button is not pressed longPress = 0 and pressDuration = 0
}
uint8_t pb1 = 0, pb2 = 0, pb3 = 0;
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    IFS1bits.CNIF = 0; 
    CN_flag = 1;
    TMR1 = 0;
    T1CONbits.TON = 1;
}
