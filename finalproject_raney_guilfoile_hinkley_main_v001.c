#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "xc.h"
#include "finalProject_asmLib.h"
#include "sensorLib_CTI.h"

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


void setup(void){
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0xFFFE;
    TRISBbits.TRISB8 = 0; // sets RB8 as output(FOR USELESS TEST BITCH LED)
}

void delay(int delay_in_ms){
    for (int i = 0; i < delay_in_ms; i++){
        wait1ms();
    }
}

/*
For minimally working hardware, color sensor is set 
to detect red values in hand and LED will turn off 
if red is detected as shown in video
*/
int main(){
    setup();
    sensor_init();
    delay(200);
    int enable = i2c_read8bits(0x00);
    int redVal = 0;
    int clrVal = 0;
    int ID;
    unsigned int RED;
    
    while(1){
        redVal = i2c_read16bits(TCS34725_RDATAL);
        clrVal = i2c_read16bits(TCS34725_CDATAL);
        RED = (redVal/clrVal) * 255;
        ID = i2c_read8bits(0x12); // checking ID register to make sure read is working correctly, should return 0x44
        delay(20);
        LATBbits.LATB8 = 0;
        if (RED >= 255){
            LATBbits.LATB8 = 1;
            delay(200);
        }
        exitWait();
        
    }
}

