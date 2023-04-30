/* 
 * File:   Joystick_Servo_Main_v001.c
 * Author: Isaiah Guilfoile
 *
 * Created on April 19, 2023, 11:56 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "xc.h"
#include "servo_lib_v001.h"

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

volatile unsigned int stickVal_horiz=0; // Contains Digital Value of Joystick Position

void delay_ms(unsigned int ms) {
    while(ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

// Middle joystick position digital value: 484 ~ 486
// Update Horizontal Position Variable with every ADC reading
void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void) {
    stickVal_horiz = ADC1BUF0;
    _AD1IF = 0; // Reset IF
}

void pic24init(void) {
    _RCDIV = 0; // set frequency to 16 MHz
    AD1PCFG = 0xffff; // set all pins digital
    TRISBbits.TRISB7 = 0; // set RB7 to output
    TRISBbits.TRISB8 = 0; // set RB8 to output
}

// Initialize ADC for horizontal joystick input on PIC24
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
    T3CONbits.TCKPS = 0b11; // Sample approximately 40 times per second
    PR3 = 1499;
    T3CONbits.TON = 1;
}

int main(void) {
    // Call Setup Functions:
    pic24init();
    adc_init();
    initServo();
    
    // servoPos range: 1000 ~ 5000
    // Middle servoPos: 3000
    // 180 Degrees of servo movement
    unsigned int servoPos = 3000; // Initialize Servo to middle position for start of game
    
    while(1) {
        // Slow RIGHT
        if ((stickVal_horiz > 500) && (stickVal_horiz < 800)) {
            LATBbits.LATB8 = 0;
            LATBbits.LATB7 = 1;
            setServo(servoPos -= 15);
            if (servoPos < 1000) {
                servoPos = 1000;
            }
        }
        // Fast RIGHT
        else if (stickVal_horiz > 800) {
            LATBbits.LATB8 = 0;
            LATBbits.LATB7 = 1;
            setServo(servoPos -= 50);
            if (servoPos < 1000) {
                servoPos = 1000;
            }
        }
        
        // Slow LEFT
        else if ((stickVal_horiz < 470) && (stickVal_horiz > 250)) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
            setServo(servoPos += 15);
            if (servoPos > 5000) {
                servoPos = 5000;
            }
        }
        // Fast LEFT
        else if(stickVal_horiz < 250) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
            setServo(servoPos += 50);
            if (servoPos > 5000) {
                servoPos = 5000;
            }
        }
        
        // No Joystick input
        else {
            LATBbits.LATB7 = 1;
            LATBbits.LATB8 = 1;
        }
        delay_ms(10);
    } 
    
    return 0; // Never reached
}
