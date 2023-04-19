/*
 * File:   hymes019_lab04_main_v001.c
 * Author: gahym
 *
 * Created on February 25, 2023, 9:06 PM
 */


#include "xc.h"

void setServo(int Val);
void initServo();
void delayms(unsigned int ms);
void initPushButton(void);

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
#define BUFFSIZE 3

void setup(void)
{
    // execute once code goes here
    CLKDIVbits.RCDIV = 0;  //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0xffff;            //sets all pins to digital I/O
    TRISA = 0b0000000000011111;  //set port A to inputs, 
    TRISB = 0b0000000000000011;  //and port B to outputs
    LATA = 0xffff;               //Set all of port A to HIGH
    LATB = 0xffff;               //and all of port B to HIGH
    
    

}

volatile int overflow = 0;
long int time_click1, time_click2, time_click3, currtime = 0;
volatile long int curPeriod = 0;
volatile int prevAction = 0;

// simple variable for debugging purposes
volatile long int pretendBuffer = 0;

// buffer components 
volatile unsigned int recentpress = 2;
volatile unsigned int olderpress = 1;
volatile unsigned int oldestpress = 0;
volatile long int buffer[BUFFSIZE];




int main(void) {
    
    // setup
    setup();
    initServo();
    initPushButton();
    
    // button states
    int curState = 0;
    int prevState = 0;
    
    setServo(0.09*PR3); // initial servo state
    
    // repeat this code forever
    while(1) {
        
        // gathers most recent data from button and the buffer
        curState = !PORTBbits.RB8;        
        time_click3 = buffer[recentpress]; //time click 2
        time_click2 = buffer[olderpress]; // time click 1
        time_click1 = buffer[oldestpress]; // time click 3
        
        // determines the current time since startup
        currtime = (TMR2 + ((long int)(PR2+1)*overflow));
        
        // difference between the 3rd and 1st click
        long int difference = time_click3 - time_click1;
        
        // if the button has been pressed
        if (curState == 1 && prevState == 0) {
            
            // if time between the third and first click in less than 25000
            // then the servo should be moved to right position
            if(difference < 25000) {
                setServo(0.06*PR3);
            }    
        }
        
        // if time between now and most recent push is > 2 seconds
        // reset servo position to the left
        else if (currtime - time_click3 > 125000) {
            setServo(0.09*PR3);
        }

        prevState = curState;       
        delayms(2);
    }
  
    return 0;
}

void initServo() {
    TRISBbits.TRISB6 = 0; // set RB6 as output
    
    // map output compare to pin RB6
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR3bits.RP6R = 18;  // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    OC1CON = 0;
    OC1CONbits.OCTSEL = 0b1;
    OC1CONbits.OCM = 0b110;
    
    OC1R =  10000 - 1;
    OC1RS = 10000 - 1;
    
    T3CON = 0;
    T3CONbits.TCKPS = 0b01; // pre-scaler of 8
    TMR3 = 0;
    PR3 = 40000 - 1; // 20 ms
    
    T3CONbits.TON = 1;    
}

void setServo(int Val) {
    OC1RS = Val - 1;
}

void initPushButton(void) {
    
        // map output compare to pin RB6
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
     RPINR7bits.IC1R = 8;  // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
     
    TRISBbits.TRISB8 = 1; // enable pin input on RB8
    CNPU2bits.CN22PUE = 1; // set RB8 pull up resistor
    
    IC1CON = 0; // Turn off and reset internal state of IC1
    IC1CONbits.ICTMR = 1; // Use Timer 2 for capture source
    IC1CONbits.ICM = 0b001; // Turn on and capture every rising and falling edge

    // timer 2 configuration
    T2CON = 0;
    T2CONbits.TCKPS = 0b11;
    PR2 = 62499;
    TMR2 = 0;
    
    _T2IE = 1; // timer 2 interrupt enable
    _T2IF = 0; // timer 2 interrupt flag
    
    _IC1IE = 1; // input capture interrupt enable
    _IC1IF = 0; // input capture interrupt flag
    
    T2CONbits.TON = 1;     
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt(void) {
    _T2IF = 0;
    overflow++;
}

void __attribute__((interrupt, auto_psv)) _IC1Interrupt(void) {
    
    static unsigned long int prevEdge = 0;
    long int curEdge;
       
    _IC1IF = 0; // reset input capture interrupt flag
    
    unsigned int temp = (int) IC1BUF; // temporary variable to store data inside the buffer

    curEdge = (temp + ((long int)(PR2+1)*overflow));
    curPeriod = curEdge - prevEdge;
    
    // resets buffer 
    IC1CONbits.ICM = 0b000;
    IC1CONbits.ICM = 0b001;
    
    // debouncing step, if the period between two edges is too small its likely a bounce
    if (curPeriod < 125) {
        return;
    }
       
    prevEdge = curEdge;
    
    // if the previous action was a release the current one is a press
    // this means the current time is good to be saved
    if (prevAction == 0) {
        prevAction = 1;
        
        buffer[oldestpress] = curEdge;
                
        // iterates the pointers for the buffer
        recentpress = (recentpress + 1) % BUFFSIZE;
        olderpress = (olderpress + 1) % BUFFSIZE;
        oldestpress = (oldestpress + 1) % BUFFSIZE;
           
    // if the previous action was a press the current is a release
    // this means current time should not be saved
    } else if (prevAction == 1) {
        prevAction = 0;
        return;
    }    
}

void delayms(unsigned int ms) {
    int i;
    for (i = 0; i < ms; i++) {
        asm("repeat #15993");
        asm("nop");
    }
    return;
}
