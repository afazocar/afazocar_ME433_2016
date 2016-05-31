#ifndef spiSteup
#define spiSteup

#include<xc.h>
#include<sys/attribs.h>

unsigned char spiIO(unsigned char x);
void initSPI();
void setVoltage(char channel, unsigned char voltage);

#endif