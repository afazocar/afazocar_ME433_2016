#include "spiSetup.h"

void initSPI() {
    // PIN ASSIGNMENTS 
    TRISBbits.TRISB15   = 0;        // Slave Select 1 pin - output
    SDI1Rbits.SDI1R     = 0b0100;   // SDI1 - RPB8
    RPB13Rbits.RPB13R   = 0b0011;   // SDO1 - RPB13
    SS1                 = 1;        // SS1 to high - no communication 
   
    //SPI1 SFRS
    SPI1CON             = 0;    
    SPI1CONbits.MSSEN   = 0;    
    SPI1CONbits.CKP     = 0;    
    SPI1CONbits.CKE     = 1;    
    SPI1CONbits.MSTEN   = 1;    
    SPI1CONbits.SMP     = 1;    
    SPI1STATbits.SPIROV = 0;    
    SPI1BUF;                    
    SPI1BRG             = 0x49; 
    SPI1CONbits.ON      = 1;    
}
void setVoltage(unsigned char channel, unsigned char voltage) {
    char byte1, byte2;
    byte1 = (channel<<7)|(0x7<<4)|(voltage>>4);
    byte2 = voltage<<4;
    SS1 = 0;
    spiIO(byte1);
    spiIO(byte2);
    SS1 = 1;
}

unsigned char spiIO(unsigned char buf) {
  SPI1BUF = buf;
  while(!SPI1STATbits.SPIRBF) {
      ;
  }
  return SPI1BUF;
}