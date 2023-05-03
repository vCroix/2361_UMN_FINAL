#include "xc.h"
#include "alphaLED_CTI.h"
#include "alphaPIC_asmLib.h"

void writeColor(int r, int g, int b){
    for (int i = 8; i > 0; i--){
        if (r & 0x80){ // 1000000 00111100
            write_1();
        }
        else{
            write_0();
        }
        r <<= 1;
    }
    for (int j = 8; j > 0; j--){
        if (g & 0x80){
            write_1();
        }
        else{
            write_0();
        }
        g <<= 1;
    }
    for (int k = 8; k > 0; k--){ 
        if (b & 0x80){
            write_1();
        }
        else{
            write_0();
        }
        b <<= 1;
    }
    wait100us();
}

unsigned long int packColor(unsigned char Red, unsigned char Grn, unsigned char Blu){
    unsigned long int RGBval = ((long) Red << 16) | ((long) Grn << 8) | ((long) Blu);
    return RGBval;
}

unsigned char getR(unsigned long int RGBval){
    unsigned char Red = (unsigned char) (RGBval >> 16);
    return Red;
}

unsigned char getG(unsigned long int RGBval){
    unsigned char Green = (unsigned char) (RGBval >> 8);
    return Green;
}

unsigned char getB(unsigned long int RGBval){
    unsigned char Blue = (unsigned char) (RGBval >> 0);
    return Blue;
}

void writePacCol(unsigned long int PackedColor){
    writeColor(getR(PackedColor), getG(PackedColor), getB(PackedColor));
}

unsigned long int Wheel(unsigned char WheelPos){
    WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return packColor(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return packColor(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return packColor(WheelPos * 3, 255 - WheelPos * 3, 0);
}
