#include "xc.h"
#include "sensorLib_CTI.h"

// Exit internal interrupt sequence in Flora Color Sensor
void i2c_exitInterrupt(void) {
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = TCS34725_ADDRESS_WRITE;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = 0b11100110;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
}
    
// Write 8-bit data package to Flora color sensor
// regAddr represents the target register written to
// Package represents 8-bit data package
void i2c_write(char regAddr, char Package){
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = TCS34725_ADDRESS_WRITE;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = regAddr | 0b10000000;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = Package;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;   
    
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
}

// Read 16-bit data package from Flora color sensor
// lowColorReg is the first 8-bit register read from
// The next read will target the register in Flora color sensor
// Intended for use on high and low color registers. First read contains lower 8 bits.
// return 16-bit package of combined data reads
char i2c_read16bits(char lowColorReg){
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1);
    IFS3bits.MI2C2IF = 0;
    
    I2C2TRN = TCS34725_ADDRESS_WRITE;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    // Send address of target read register
    I2C2TRN = lowColorReg | 0b10100000;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2CONbits.RSEN = 1;
    while(I2C2CONbits.RSEN == 1);
    I2C2TRN = TCS34725_ADDRESS_READ;
    while(I2C2STATbits.TRSTAT);
    I2C2CONbits.RCEN = 1;
    
    // Save data from buffer register once all bits have been received
    while(!I2C2STATbits.RBF);
    int lowBits = I2C2RCV;
    
    // Send NACK acknowledge from master device
    I2C2CONbits.ACKDT = 1;
    I2C2CONbits.ACKEN = 1;
    while(I2C2CONbits.ACKEN);
    I2C2CONbits.RCEN = 1;
    
    // Save data from buffer register once all bits have been received
    while(!I2C2STATbits.RBF);
    int highBits = I2C2RCV;
    
    // Send NACK acknowledge from master device
    I2C2CONbits.ACKDT = 1;
    I2C2CONbits.ACKEN = 1;
    while(I2C2CONbits.ACKEN);
    
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);    
    
    return (((highBits) << 8) | lowBits);
}

// Perform 8-bit read sequence from Flora color sensor and return data
// regAddr represents the address of the register read from in Flora color sensor
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

// Initialize I2C operation on PIC24 and power on/ enable Flora color sensor
// Must be called before Flora color sensor can be used.
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
    i2c_write(TCS34725_ATIME, 0xC0); // Integration time: 
    i2c_write(TCS34725_CONTROL, 0x11);
    i2c_write(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    
    // Delay for ADC integration time
    delay((256 - 0xC0) * 12 / 5 + 1);
}

// Exit Idle state in Flora Color Sensor to re-enable data reading and ADC integration
void exitIdle(void) {
    i2c_write(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

// Convert raw 16-bit color readings to uniform values from 0~255 and return new value
// After conversion, a high value indicates more of that color is present in front of sensor
int getRGB(float colorRaw, float clearRaw) {
    float sum = clearRaw;
    if (clearRaw == 0) {
        return -1; // Return -1 for Black
    }
    return (int)((colorRaw / sum) * 255.0);
}
