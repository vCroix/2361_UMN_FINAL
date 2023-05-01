#include "xc.h"
#include "joystick_lib_v001.h"
#include "servo_lib_v001.h" // This library is reliant on our servo motor library

/*
 * Analog Joystick Library
 * Performs Analog to Digital Conversion on Joystick pin output voltage
 * Determine Joystick Position quickly without observable input delay
 * Move a SG90 9g microservo motor based on joystick position 
 */

// Contains Digital Value of Joystick Position
// Middle joystick position digital value: 484 ~ 486
volatile unsigned int stickVal_horiz = 0;

// Update Horizontal Position Variable with every ADC reading
void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void) {
    stickVal_horiz = ADC1BUF0;
    _AD1IF = 0; // Reset IF
}

// Initialize ADC for horizontal joystick input on PIC24
// Must be called in main before any operation
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

// Move the servo motor based on the magnitude of joystick position
// Outputs RB7 and RB8 are used to control LED indicators primarily used for debugging
// servoPosition is not allowed outside of operating range.
unsigned int updateServo(unsigned int servoPosition) {
    // Slow RIGHT
    if ((stickVal_horiz > 500) && (stickVal_horiz < 800)) {
            LATBbits.LATB8 = 0; // 
            LATBbits.LATB7 = 1;
            setServo(servoPosition -= 15);
            if (servoPosition < 1000) {
                servoPosition = 1000;
            }
        }
        // Fast RIGHT
        else if (stickVal_horiz > 800) {
            LATBbits.LATB8 = 0;
            LATBbits.LATB7 = 1;
            setServo(servoPosition -= 50);
            if (servoPosition < 1000) {
                servoPosition = 1000;
            }
        }
        
        // Slow LEFT
        else if ((stickVal_horiz < 470) && (stickVal_horiz > 250)) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
            setServo(servoPosition += 15);
            if (servoPosition > 5000) {
                servoPosition = 5000;
            }
        }
        // Fast LEFT
        else if(stickVal_horiz < 250) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
            setServo(servoPosition += 50);
            if (servoPosition > 5000) {
                servoPosition = 5000;
            }
        }
        
        // No Joystick input
        else {
            LATBbits.LATB7 = 1;
            LATBbits.LATB8 = 1;
        }
}
