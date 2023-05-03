/* 
 * File:   Joystick_main_v001.c
 * Author: Isaiah Guilfoile
 *
 * Created on April 19, 2023, 11:56 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "xc.h"
#include "servo_lib_v001.h"
#include "joystick_lib_v001.h"

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

void delay_ms(unsigned int ms) {
    while(ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

void pic24init(void) {
    _RCDIV = 0; // set frequency to 16 MHz
    AD1PCFG = 0xffff; // set all pins digital
    TRISBbits.TRISB7 = 0; // set RB7 to output
    TRISBbits.TRISB8 = 0; // set RB8 to output
}

int main(void) {
    pic24init();
    adc_init();
    initServo();
    
    while(1) {
      // Move the servo motor based on the magnitude and direction of joystick position
      // Outputs RB7 and RB8 are used to control LED indicators primarily used for debugging
      // servoPosition is not allowed outside of operating range.
       
      // Slow RIGHT
      if ((stickVal_horiz > 500) && (stickVal_horiz < 800)) {
            LATBbits.LATB8 = 0; // RB8 indicates RIGHT input
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
            LATBbits.LATB7 = 0; // RB7 indicates LEFT input
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
            LATBbits.LATB7 = 1; // Both LEDs off
            LATBbits.LATB8 = 1;
        }
      
        delay_ms(10); // Small Delay to make servo motion appear smoother
    } 
  
    return 0; // Never reached
}

