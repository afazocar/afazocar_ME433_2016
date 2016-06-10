#ifndef DAC_H
#define DAC_H
#include <xc.h>

#define A   (unsigned char) 0x00
#define B   (unsigned char) 0x01
#define SS1 LATBbits.LATB15

void initSPI();
void setVoltage(unsigned char channel, unsigned char voltage);
unsigned char spiIO(unsigned char write);

#endif