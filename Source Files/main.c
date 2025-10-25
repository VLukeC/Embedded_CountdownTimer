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
#include "countdown.h"
// Global Variables
volatile uint8_t longPress = 0; // Flag for the long press
volatile uint8_t pressActive = 0; //Checks if any button is pressed
volatile uint8_t pressDone = 0; // Flag for button released
uint8_t pb1 = 0, pb2 = 0, pb3 = 0;
uint8_t running = 0; // false by default, flag for if the clock has started or not
uint8_t paused = 0;  // Flag for if the clock has been paused
uint8_t T1flag = 0;    // T1 Interrupt flag
uint8_t T2flag = 0;     // T2 Interrupt flag
uint8_t CNflag = 0;     // CN Interrupt flag
uint8_t pb1Event = 0, pb2Event = 0, pb3Event = 0; //Flag for PB events
uint8_t alarm = 0;              // Alarm active Flag




// Main Program
int main(void) {
    AD1PCFG = 0xFFFF; /* keep this line as it sets I/O pins that can also be analog to be digital */

    /* Initializations */  
    
    // === TIMER 1: 1-second periodic timer ===
    T1CONbits.TON = 0;        // Off initially
    T1CONbits.TCS = 0;        // Internal clock (Fcy)
    T1CONbits.TCKPS = 3;      // 1:256 prescaler
    T1CONbits.TSIDL = 0;      // Continue in Idle
    IPC0bits.T1IP = 2;        // Priority
    IFS0bits.T1IF = 0;        // Clear interrupt flag
    IEC0bits.T1IE = 1;        // Enable interrupt
    PR1 = 450;                  //~.5 seconds
    TMR1 = 0;
    
    
    // === Timer 2: 1 second update timer for countdown ===
    T2CONbits.T32 = 0;      // Off initially
    T2CONbits.TCKPS = 3;    // 1:256 prescalar
    T2CONbits.TCS = 0;      // Internal clock
    T2CONbits.TSIDL = 0;    //Continue in idle
    IPC1bits.T2IP0 = 7;     // Priority
    IEC0bits.T2IE = 1;      // Enable Inerrupt
    IFS0bits.T2IF = 0;      // Clear flag
    PR2= 977;               //1 sec period
    TMR2 = 0;               
    
    // === TIMER 3: CNDebounce ===
    T3CONbits.TON = 0;       // keep off until needed
    T3CONbits.TCS = 0;       // internal clock
    T3CONbits.TCKPS = 3;     // 1:256 prescale
    T3CONbits.TSIDL = 0;     // run in idle mode
    IPC2bits.T3IP = 2;       // priority = 2
    IFS0bits.T3IF = 0;       // clear flag
    IEC0bits.T3IE = 1;  // enable interrupt
    PR3 = 30;           
    TMR3 = 0;
      
    IOinit(); // Initilize pins and CN interrupts
    newClk(500);    // change clock frequency to 500kHz
  
    DispTime(min,sec); // Starting Time
    Disp2String("               "); //clear line
    //Main Loop
    while(1) {
        Idle(); // Wait for interrupts
        if (pressActive) {
            IOcheck();  // Process button input
        }
        if(running){
            updateTimer();  // While the clock is running run the update timer function
        }
        if(alarm) alarm_flash();    // When alarm flag is raised, run this function
    }
    
    return 0;
}

// Timer 1
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0; 
    TMR1 = 0;
    T1flag = 1;
    static uint8_t long_counter;
    // Every 3 half seconds set the longPress flag
    if(++long_counter >= 3){
        longPress = 1;
        long_counter = 0;
    }
}



// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0;
    TMR2 = 0;
    T2flag = 1;
}

// Timer 3 de-bounce
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    IFS0bits.T3IF = 0;  //Clear interrupt flag
    T3CONbits.TON = 0; // Stop debounce timer
    if(CNflag){
        CNflag = 0; //Clear CN flag

        // Read states of button
        pb1 = !PORTBbits.RB7;
        pb2 = !PORTBbits.RB4;
        pb3 = !PORTAbits.RA4;
        // Check if any button has been pressed
        uint8_t anyPressed = pb1 || pb2 || pb3;
        if (anyPressed) {
            // Record which button(s) triggered
            if (pb1) pb1Event = 1;
            if (pb2) pb2Event = 1;
            if (pb3) pb3Event = 1;
            pressActive = 1;
            longPress = 0;
            
            // Increment_delay
            TMR1 =0;
            T1CONbits.TON = 1;
            T1flag = 1; //for initial presses
        }
        else if (pressActive) {
            // If button is released stop tracking
            longPress = 0;
            pressActive = 0;
            T3CONbits.TON = 0;
            T1CONbits.TON = 0;
        }
    }
}


void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    IFS1bits.CNIF = 0;
    
    //Snooze alarm if on
    if(alarm){
        LATBbits.LATB9 = 0;
        LATAbits.LATA6 = 0;
        alarm = 0;
        DispTime(min,sec);
        Disp2String("\r                       "); // clear alarm
    }
    // Sets flag and timer - inputs handled by T3Interrupt
    CNflag = 1;
    TMR3 = 0;
    T3CONbits.TON = 1;
}

