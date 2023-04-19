/* 
 * File:   Joystick_main_v001.c
 * Author: isaiahguilfoile
 *
 * Created on April 19, 2023, 11:56 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "xc.h"

// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

volatile int stickVal=0;

void delay_ms(unsigned int ms) {
    while(ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

// Middle position digital value: 484 ~ 486
void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void) {
    stickVal = ADC1BUF0;
    _AD1IF = 0; // Reset IF
}

void pic24init(void) {
    _RCDIV = 0; // set frequency to 16 MHz
    AD1PCFG = 0xffff; // set all pins digital
    TRISBbits.TRISB7 = 0; // set RB7 to output
    TRISBbits.TRISB8 = 0; // set RB8 to output
}

void adc_init(void) {
    TRISAbits.TRISA0 = 1;   // set RA0 to input
    AD1PCFGbits.PCFG0 = 0;  // Setup I/O
    
    AD1CON2bits.VCFG = 0b000;   // Use AVDD (3.3V)and AVSS (0 V) as max and min
    AD1CON3bits.ADCS = 0b00000001;     // We want TAD >= 75 ns (Tcy = 62.5 ns)
    AD1CON1bits.SSRC = 0b010;     // Sample on timer 3 events
    AD1CON3bits.SAMC = 0b00001;     // You want at least 1 sample time bit
    AD1CON1bits.FORM = 0b00;     // Data output form -- recommend unsigned int
    
    AD1CON1bits.ASAM = 0b1;     // Read the reference manual to see what this does
    AD1CON2bits.SMPI = 0b0000;     // Read the reference manual to see what this does
    AD1CON1bits.ADON = 1;       // Turn on the ADC
    
    _AD1IF = 0;         // Clear IF
    _AD1IE = 1;         // Enable interrupts
    
    TMR3 = 0;           // Setup timer 3
    T3CON = 0;
    T3CONbits.TCKPS = 0b01;
    PR3 = 15624;
    T3CONbits.TON = 1;
}

int main(void) {
    pic24init();
    adc_init();
    while(1) {
        if (stickVal > 490) {
            LATBbits.LATB8 = 0;
            LATBbits.LATB7 = 1;
            delay_ms(10);
        }
        else if (stickVal < 480) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
            delay_ms(10);
        }
        else {
            LATBbits.LATB7 = 1;
            LATBbits.LATB8 = 1;
            delay_ms(10);
        }
    }
    return 0;
}
/*
    volatile int servoPos

while(fast zone) {
    setServo(Pos)
    ServoPos += 100;
}

while (slow zone) {
    setServo(servoPos) 
    ServoPos += 10;
}
 */

