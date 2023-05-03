#include "xc.h"
#include "joystick_servoLib_CTI.h"

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

// Incrememnt the servo position a small amount right
void servoRight_fast(void) {
    setServo(servoPosition -= 15);
    if (servoPosition < 1000) {
        servoPosition = 1000;
    }
}

// Incremement servo position a large amount right
void servoRight_slow(void) {
    setServo(servoPosition += 15);
    if (servoPosition < 1000) {
        servoPosition = 1000;
    }
}

// Increment servo position a small amount left
void servoLeft_slow(void) {
    setServo(servoPosition += 15);
    if (servoPosition < 1000) {
        servoPosition = 1000;
    }
}

// Increment servo position a large amount left
void servoLeft_fast(void) {
    setServo(servoPosition += 15);
    if (servoPosition < 1000) {
        servoPosition = 1000;
    }
}

// Move the servo motor based on the magnitude of joystick position
// Outputs RB7 and RB8 are used to control LED indicators primarily used for debugging
// servoPosition is not allowed outside of operating range.
unsigned int updateServo(unsigned int servoPosition) {
    // Slow RIGHT
    if ((stickVal_horiz > 500) && (stickVal_horiz < 800)) {
            LATBbits.LATB8 = 0; // 
            LATBbits.LATB7 = 1;
            servoRight_slow();
        }
        // Fast RIGHT
        else if (stickVal_horiz > 800) {
            LATBbits.LATB8 = 0;
            LATBbits.LATB7 = 1;
            servoRight_fast();
        }
        
        // Slow LEFT
        else if ((stickVal_horiz < 470) && (stickVal_horiz > 250)) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
            servoLeft_slow()
        }
        // Fast LEFT
        else if(stickVal_horiz < 250) {
            LATBbits.LATB7 = 0;
            LATBbits.LATB8 = 1;
           servoLeft_fast();
        }
        
        // No Joystick input
        else {
            LATBbits.LATB7 = 1;
            LATBbits.LATB8 = 1;
        }
}

// Initialize servo motor for proper use
void initServo(void) {
    // Use pin B6 as a PWM pin. This pin needs to be an output
    // and have the output compare peripheral mapped to it. 
    
    TRISBbits.TRISB6 = 0;   // Setting pin at B6 as output
    
    // Map output compare to pin B6
    __builtin_write_OSCCONL(OSCCON & 0xBF); // this line forces bit 6 to be a 0 on OSCCON
    RPOR3bits.RP6R = 18;    // this line sets pin RP6/RB6 to output compare
    __builtin_write_OSCCONL(OSCCON | 0x40); // this line forces bit 6 to be a 1 on OSCCON

    OC1CON = 0;
    OC1CONbits.OCTSEL = 0b1;
    OC1CONbits.OCM = 0b110;
    
    T3CON = 0;
    T3CONbits.TCKPS = 0b01;
    TMR3 = 0;
    PR3 = 40000 - 1; // 20 ms
    
    T3CONbits.TON = 1; // Turn on timer
}

// Adjust the servo position by changing the duty cycle of PWM signal
void setServo(int Val) {
    OC1RS = Val;    // set shadow register to add level of protection. Will update real register automatically
}
