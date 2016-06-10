#include <xc.h>
#include "i2cSetup.h"

char i2cMasterRead(char reg_addr) {
    char readMaster;
    i2cMasterStart();
    i2cMasterSend((SLAVE_ADDR << 1) | 0);  
    i2cMasterSend(reg_addr);
    i2cMasterRestart();                     
    i2cMasterSend((SLAVE_ADDR << 1) | 1);   
    readMaster = i2cMasterRecv();           
    i2cMasterAck(1);
    i2cMasterStop();
    return readMaster;
}

void i2cMasterWrite(char reg_addr, char byte) {
    i2cMasterStart();
    i2cMasterSend((SLAVE_ADDR << 1) | 0);
    i2cMasterSend(reg_addr);
    i2cMasterSend(byte);
    i2cMasterStop();
}

void i2cMasterSetup(void) {         // set up I2C2 as a master, at 100 kHz
  I2C2BRG = 233;
  I2C2CONbits.ON = 1;
}

void i2cMasterStart(void) {         // send a START signal
    I2C2CONbits.SEN = 1;            
    while(I2C2CONbits.SEN) {
        ; 
    }
}

void i2cMasterRestart(void) {       // send a RESTART signal
    I2C2CONbits.RSEN = 1;         
    while(I2C2CONbits.RSEN) {
        ; 
    }
}

void i2cMasterSend(unsigned char byte) {    // send a byte (either an address or data)
  I2C2TRN = byte;                          
  while(I2C2STATbits.TRSTAT) { 
      ; 
  } 
  while(I2C2STATbits.ACKSTAT) {
      ;
  }
}

unsigned char i2cMasterRecv(void) {         // receive a byte of data
    I2C2CONbits.RCEN = 1;           
    while(!I2C2STATbits.RBF) { 
        ; 
    } 
    return I2C2RCV;                
}

void i2cMasterAck(int val) {        // send an ACK (0) or NACK (1)
    I2C2CONbits.ACKDT = val;
    I2C2CONbits.ACKEN = 1;
    while(I2C2CONbits.ACKEN) { 
        ; 
    }
}

void i2cMasterStop(void) {          // send a stop
  I2C2CONbits.PEN = 1;             
  while(I2C2CONbits.PEN) { 
      ; 
  }
}