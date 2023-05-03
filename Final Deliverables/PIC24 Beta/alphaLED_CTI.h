/* 
 * File:   alphaLED_CTI.h
 * Author: Croix
 *
 * Created on April 26, 2023, 11:53 AM
 */

#ifndef ALPHALED_CTI_H
#define	ALPHALED_CTI_H

#include "xc.h"
#include "sensorLib_CTI.h"
#include "alphaPIC_asmLib.h"

#ifdef	__cplusplus
extern "C" {
#endif

    void writeColor(int r, int g, int b);
    unsigned long int packColor(unsigned char Red, unsigned char Grn, unsigned char Blu);
    unsigned char getR(unsigned long int RGBval);
    unsigned char getG(unsigned long int RGBval);
    unsigned char getB(unsigned long int RGBval);
    void writePacCol(unsigned long int PackedColor);
    unsigned long int Wheel(unsigned char WheelPos);

#ifdef	__cplusplus
}
#endif

#endif	/* ALPHALED_CTI_H */

