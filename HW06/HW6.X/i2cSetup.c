#include <xc.h>
#include "i2cSetup.h"

char i2cMasterRead(char device, char reg_addr) {
    char readMaster;
    i2cMasterStart();
    i2cMasterSend((device << 1) | 0);
    i2cMasterSend(reg_addr);
    i2cMasterRestart();             
    i2cMasterSend((device << 1) | 1);
    readMaster = i2cMasterRecv();    
    i2cMasterAck(1);
    i2cMasterStop();
    return readMaster;
}

void i2cMasterReadAll(char device,char start_reg,int numval,unsigned char* array) {
    int counter=0;
    i2cMasterStart();
    i2cMasterSend((device << 1) | 0);
    i2cMasterSend(start_reg);
    i2cMasterRestart();          
    i2cMasterSend((device << 1) | 1);
    for(;counter<numval;counter++) {
        array[counter] = i2cMasterRecv();
        i2cMasterAck(0);
    }
    array[numval] = i2cMasterRecv();
    i2cMasterAck(1);
    i2cMasterStop();
}

void i2cMasterWrite(char device, char reg_addr, char byte) {
    i2cMasterStart();
    i2cMasterSend((device << 1) | 0);
    i2cMasterSend(reg_addr);
    i2cMasterSend(byte);
    i2cMasterStop();
}

void i2cMasterSetup(void) {
  ANSELBbits.ANSB2 = 0;                 // I2C2 analog off;
  ANSELBbits.ANSB3 = 0;                 // I2C2 analog off;
  I2C2BRG = 233; 
  I2C2CONbits.ON = 1;                  
  i2cMasterWrite(IMU_ADDR,CTRL1_XL,CTRL1_VAL);    // Setup the XL register
  i2cMasterWrite(IMU_ADDR,CTRL2_G ,CTRL2_VAL);    // Setup the G register
  i2cMasterWrite(IMU_ADDR,CTRL3_C ,CTRL3_VAL);    // Setup the C register
}

void i2cMasterStart(void) {
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN) { 
        ; 
    }
}

void i2cMasterRestart(void) {     
    I2C2CONbits.RSEN = 1;
    while(I2C2CONbits.RSEN) { 
        ; 
    }
}

void i2cMasterSend(unsigned char byte) {
  I2C2TRN = byte;                           
  while(I2C2STATbits.TRSTAT) { 
      ; 
  }          
  while(I2C2STATbits.ACKSTAT) {
      ;
  }
}

unsigned char i2cMasterRecv(void) {
    I2C2CONbits.RCEN = 1;          
    while(!I2C2STATbits.RBF) { 
        ; 
    } 
    return I2C2RCV;               
}

void i2cMasterAck(int val) {
    I2C2CONbits.ACKDT = val;        
    I2C2CONbits.ACKEN = 1;        
    while(I2C2CONbits.ACKEN) { 
        ; 
    } 
}

void i2cMasterStop(void) {         
  I2C2CONbits.PEN = 1;              
  while(I2C2CONbits.PEN) { 
      ; 
  }  
}