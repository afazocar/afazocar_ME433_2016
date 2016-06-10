#include <xc.h>
#include "i2cSetup.h"

char i2cMasterRead(char device, char reg_addr)
{
    char readMaster;
    i2cMasterStart();
    i2cMasterSend((device << 1) | 0);   // writing
    i2cMasterSend(reg_addr);
    i2cMasterRestart();                     // send a RESTART to read
    i2cMasterSend((device << 1) | 1);   // reading
    readMaster = i2cMasterRecv();           // receive a byte from the bus
    i2cMasterAck(1);
    i2cMasterStop();
    return readMaster;
}

void i2cMasterReadAll(char device,char start_reg,int numval,unsigned char* array)
{
    int counter=0;
    i2cMasterStart();
    i2cMasterSend((device << 1) | 0); // writing
    i2cMasterSend(start_reg);
    i2cMasterRestart();               // send a RESTART to read
    i2cMasterSend((device << 1) | 1); // reading
    for(;counter<numval;counter++)
    {
        array[counter] = i2cMasterRecv();    // receive a byte from the bus
        i2cMasterAck(0);
    }
    array[numval] = i2cMasterRecv();
    i2cMasterAck(1);
    i2cMasterStop();
}

void i2cMasterWrite(char device, char reg_addr, char byte)
{
    i2cMasterStart();
    i2cMasterSend((device << 1) | 0); // writing
    i2cMasterSend(reg_addr);
    i2cMasterSend(byte);
    i2cMasterStop();
}

void i2cMasterSetup(void) {
  ANSELBbits.ANSB2 = 0;                 // I2C2 analog off;
  ANSELBbits.ANSB3 = 0;                 // I2C2 analog off;
  I2C2BRG = 233; // for 100kHz;         // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
  I2C2CONbits.ON = 1;                   // turn on the I2C2 module
  i2cMasterWrite(IMU_ADDR,CTRL1_XL,CTRL1_VAL);    // Setup the XL register
  i2cMasterWrite(IMU_ADDR,CTRL2_G ,CTRL2_VAL);    // Setup the G register
  i2cMasterWrite(IMU_ADDR,CTRL3_C ,CTRL3_VAL);    // Setup the C register
}

// Start a transmission on the I2C bus
void i2cMasterStart(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2cMasterRestart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2cMasterSend(unsigned char byte) {    // send a byte to slave
  I2C2TRN = byte;                           // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }          // wait for the transmission to finish
  while(I2C2STATbits.ACKSTAT) {;}
}

unsigned char i2cMasterRecv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;           // start receiving data
    while(!I2C2STATbits.RBF) { ; }  // wait to receive the data
    return I2C2RCV;                 // read and return the data
}

void i2cMasterAck(int val) {        // sends ACK = 0 (slave should send another byte)
                                    // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;        // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;          // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }  // wait for ACK/NACK to be sent
}

void i2cMasterStop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;              // communication is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }      // wait for STOP to complete
}