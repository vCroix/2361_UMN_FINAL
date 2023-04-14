/* 
 * File:   sensorLib_CTI.h
 * Author: Croix
 *
 * Created on April 12, 2023, 11:46 AM
 */

#ifndef SENSORLIB_CTI_H
#define	SENSORLIB_CTI_H

#ifdef	__cplusplus
extern "C" {
#endif

    void i2c_write(char Package, char Command);
    void sensor_init(void);
    void exitWait(void);
    int i2c_read8bits(char regAddr);
    char i2c_read16bits(char lowColorReg);

#ifdef	__cplusplus
}
#endif

#endif	/* SENSORLIB_CTI_H */

