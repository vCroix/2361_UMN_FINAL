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

// PIC24 general setup
void setup(void){
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0xFFFE;
    TRISBbits.TRISB8 = 0; // sets RB8 as output
}

void delay(int delay_in_ms){
    for (int i = 0; i < delay_in_ms; i++){
        wait1ms();
    }
}

int main(){
    setup();
    sensor_init();
    delay(200);
    
    i2c_exitInterrupt();
    
    unsigned int redVal = 0;
    unsigned int greenVal = 0;
    unsigned int blueVal = 0;
    unsigned int clrVal = 0;
    int RED;
    int GREEN;
    int BLUE;
    
    int AVALID = 0;
    int deviceID = i2c_read8bits(TCS34725_ID);
    
    while(1) {
        while (AVALID == 0) {
           int STATUS = i2c_read8bits(TCS34725_STATUS);
           AVALID = STATUS & 0b00000001; // Only care about last bit 
        }
        i2c_write(TCS34725_ENABLE, 0b00001011);
        
        redVal = i2c_read16bits(TCS34725_RDATAL);
        greenVal = i2c_read16bits(TCS34725_GDATAL);
        blueVal = i2c_read16bits(TCS34725_BDATAL);
        clrVal = i2c_read16bits(TCS34725_CDATAL);
        
        // Convert raw data to RGB values for each color
        RED = getRGB(redVal, clrVal);
        GREEN = getRGB(greenVal, clrVal);
        BLUE = getRGB(blueVal, clrVal);
        
        AVALID = 0;
        i2c_write(TCS34725_ENABLE, 0b00000011);
        delay(160);
    }
}
