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
extern volatile uint8_t longPress = 0; // Flag for the long press
extern volatile uint8_t pressDuration = 0; // Every 0.5s this is incremented so when it reaches 6 it is a 3s long press
extern volatile uint8_t pressActive = 0; //Checks if any button is pressed
extern volatile uint8_t pressDone = 0;
uint8_t pb1 = 0, pb2 = 0, pb3 = 0;




/**
 * You might find it useful to add your own #defines to improve readability here
 */

int main(void) {
    AD1PCFG = 0xFFFF; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    newClk(500);

    // === TIMER 3: Press-duration timer ===
    T3CONbits.TON = 0;       // keep off until needed
    T3CONbits.TCS = 0;       // internal clock
    T3CONbits.TCKPS = 2;     // 1:64 prescale
    T3CONbits.TSIDL = 0;     // run in idle mode
    IPC2bits.T3IP = 2;       // priority = 2
    IFS0bits.T3IF = 0;       // clear flag
    IEC0bits.T3IE = 1;  // enable interrupt
    PR3 = 100; // 25 ms period
    TMR3 = 0;
    
    

   
    /* Initilizations */    
    IOinit();
  
    
    while(1) {
        
        Idle();
        // If a PB event occurs run this code
        if (pressDone) {
            IOcheck();
            pressDone = 0;
            longPress = 0;
            pressDuration = 0;
        }
    }
    
    return 0;
}




// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {

}

// Timer 3 interrupt Long vs Short button press
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    IFS0bits.T3IF = 0;
    if (pressActive) {
        pressDuration++;
        if (pressDuration >= 120) {   
            longPress = 1;
            pressDone = 1;    
            pressActive = 0;
            T3CONbits.TON = 0;
        }
    }
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    IFS1bits.CNIF = 0;
    pb1 = !PORTBbits.RB7;
    pb2 = !PORTBbits.RB4;
    pb3 = !PORTAbits.RA4;
    uint8_t anyPressed = pb1 || pb2 || pb3;
    if (anyPressed) {
        pressActive = 1;
        pressDone = 0;
        longPress = 0;
        pressDuration = 0;
        TMR3 = 0;
        T3CONbits.TON = 1;       
    }
    else if (pressActive) {
        pressActive = 0;
        pressDone = 1;
        T3CONbits.TON = 0;       
    }
}


