#ifndef I2C_SETUP_H
#define I2C_SETUP_H
#define SLAVE_ADDR (char) 0x20            // (S) -> 0100 000(R/W) -> byte

char i2cMasterRead(char reg_addr);
void i2cMasterWrite(char reg_addr, char write);
void i2cMasterSetup(void);              // set up I2C2 as a master, at 100 kHz
void i2cMasterStart(void);              // send a START signal
void i2cMasterRestart(void);            // send a RESTART signal
void i2cMasterSend(unsigned char byte); // send a byte (either an address or data)
unsigned char i2cMasterRecv(void);      // receive a byte of data
void i2cMasterAck(int val);             // send an ACK (0) or NACK (1)
void i2cMasterStop(void);               // send a stop

#endif