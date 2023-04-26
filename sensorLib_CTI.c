#include "xc.h"
#include "sensorLib_CTI.h"

unsigned volatile int redVal = 0;
unsigned volatile int greenVal = 0;
unsigned volatile int blueVal = 0;
unsigned volatile int clrVal = 0;
    
void i2c_write(char regAddr, char Package){
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = TCS34725_ADDRESS_WRITE;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = regAddr | 10000000;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = Package;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;   
    
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
}

char i2c_read16bits(char lowColorReg){
    int lowBits = 0;
    int highBits = 0;
    
    lowBits = i2c_read8bits(lowColorReg);
    highBits = i2c_read8bits(lowColorReg + 1);
    
    return (((highBits) << 8) | lowBits);
}

int i2c_read8bits(char regAddr){
    int data;
    
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = TCS34725_ADDRESS_WRITE;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    // Send address of target read register
    I2C2TRN = regAddr | 0b10000000;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2CONbits.RSEN = 1;
    while(I2C2CONbits.RSEN == 1);
    I2C2TRN = TCS34725_ADDRESS_READ;
    while(I2C2STATbits.TRSTAT);
    I2C2CONbits.RCEN = 1;
    
    // Save data from buffer register once all bits have been received
    while(!I2C2STATbits.RBF);
    data = I2C2RCV;
    
    // Send NACK acknowledge from master device
    I2C2CONbits.ACKEN = 1;
    I2C2CONbits.ACKDT = 1;
    while(I2C2CONbits.ACKEN);
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);    
    
    return data;
}

void sensor_init(void){
    // PIC24 I2C settings
    I2C2CONbits.I2CEN = 0;
    I2C2BRG = 157;
    I2C2CONbits.I2CEN = 1;
    IFS3bits.MI2C2IF = 0;
    
    // Power Sensor on then set small delay
    i2c_write(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    delay(3);
    
    // Set ADC integration time/gain, then enable sensor to start getting data
    i2c_write(TCS34725_ATIME, 0xC0);
    i2c_write(TCS34725_CONTROL, 0b00000000);
    i2c_write(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    
    // Delay for ADC integration time
    delay((256 - 0xC0) * 12 / 5 + 1);
}

void exitIdle(void) {
    i2c_write(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

int getRGB(float colorRaw, float clearRaw) {
    float sum = clearRaw;
    if (clearRaw == 0) {
        return -1; // Return -1 for Black
    }
    return (int)((colorRaw / sum) * 255.0);
}

