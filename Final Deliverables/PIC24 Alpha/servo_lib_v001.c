#include "xc.h"
#include "servo_lib_v001.h"

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

void setServo(int Val) {
    OC1RS = Val;    // set shadow register to add level of protection. Will update real register automatically
}


