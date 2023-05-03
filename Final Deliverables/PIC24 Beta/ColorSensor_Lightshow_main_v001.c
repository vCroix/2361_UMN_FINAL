#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "xc.h"
#include "alphaPIC_asmLib.h"
#include "sensorLib_CTI.h"
#include "alphaLED_CTI.h"

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
    
    i2c_exitInterrupt(); // Exit internal interrupt on Flora Sensor
    
    unsigned int redVal = 0;
    unsigned int greenVal = 0;
    unsigned int blueVal = 0;
    unsigned int clrVal = 0;
    int RED;
    int GREEN;
    int BLUE;
    
    int AVALID = 0; // Avalid bit is set on Flora Sensor when ADC integration is complete
    int deviceID = i2c_read8bits(TCS34725_ID);  // Read from Device ID register in Flora Sensor to ensure proper read
                                                // Device ID should read 0x44
    
    while(1) {
        // Continuously read AVALID until it is set, indicating that ADC integration is complete on Flora Sensor
        while (AVALID == 0) {
           int STATUS = i2c_read8bits(TCS34725_STATUS);
           AVALID = STATUS & 0b00000001; // Only care about last bit 
        }
        i2c_write(TCS34725_ENABLE, 0b00001011); // Enter IDLE state in Flora Sensor to prevent further ADC integrations temporarily
        
        // Read all color registers on Flora Sensor
        redVal = i2c_read16bits(TCS34725_RDATAL);
        greenVal = i2c_read16bits(TCS34725_GDATAL);
        blueVal = i2c_read16bits(TCS34725_BDATAL);
        clrVal = i2c_read16bits(TCS34725_CDATAL);
        
        // Convert raw data to RGB values for each color (0 ~ 255)
        RED = getRGB(redVal, clrVal);
        GREEN = getRGB(greenVal, clrVal);
        BLUE = getRGB(blueVal, clrVal);
      
        if ((RED > 150) && (BLUE < 150) && (GREEN < 150)) { // This can be changed to produce accurate results depending on room lighting
            // Enter Lightshow for winner!
            while(1){
                for(i = 0; i < 255; i++) {
                    writeColor(i, 0, 255-i);
                    delay(3);
                }
                for (i = 255; i > 0; i--) {
                    writeColor(i, 0, 255-i);
                    delay(3);
                } 
            }
          
        }
        
        AVALID = 0; // Reset AVALID 
        i2c_write(TCS34725_ENABLE, 0b00000011); // Exit IDLE state in Flora Sensor
        delay(160); // Delay to allow for full integration cycle in Flora Sensor
    }
}
