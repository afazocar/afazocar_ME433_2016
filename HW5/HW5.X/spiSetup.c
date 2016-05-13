#include "spiSetup.h"

#define CS LATBbits.LATB7       // Chip Select

unsigned char spiIO(unsigned char x) {
    SPI1BUF = x;
    while(!SPI1STATbits.SPIRBF) {     // Wait for byte
      ;
    }
    return SPI1BUF;
}

  void initSPI(){
    RPB8Rbits.RPB8R = 0b0011;   // Set RPB8 to SDO1
    TRISBbits.TRISB7 = 0;       // Set the CS pin to be a digital output
    CS = 1;                     // Start CS pin hi

    SPI1CON = 0;                // Reset SPI
    SPI1BUF;                    // Clear the Rx buffer
    SPI1BRG = 11999;            // Baud rate to 10 MHz
    SPI1STATbits.SPIROV = 0;    // Clear the overflow bit
    SPI1CONbits.CKE = 1;        // Data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;      // Master operation
    SPI1CONbits.ON = 1;         // Turn on SPI
}

  void setVoltage(char channel, unsigned char voltage){
    unsigned short buffer = 0x0;
    buffer = buffer | channel << 15;
    buffer = buffer | 0x7 << 12;
    buffer = buffer | voltage << 4;

    CS = 0;             // Set CS lo
    spiIO((buffer & 0xFF00) >> 8);
    spiIO(buffer & 0x00FF);
    CS = 1;             // Set CS hi
}