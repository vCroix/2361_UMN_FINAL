/* 
 * File:   joystick_lib_v001.h
 * Author: isaiah guilfoile
 *
 * Created on April 30, 2023, 1:03 PM
 */

#ifndef JOYSTICK_LIB_V001_H
#define	JOYSTICK_LIB_V001_H

#ifdef	__cplusplus
extern "C" {
#endif

void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void);
void adc_init(void);
unsigned int updateServo(unsigned int servoPosition);



#ifdef	__cplusplus
}
#endif

#endif	/* JOYSTICK_LIB_V001_H */

