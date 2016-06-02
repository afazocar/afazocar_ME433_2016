#ifndef i2cSetup
#define i2cSetup

#include<xc.h>
#include<sys/attribs.h>

// ************************** I2C Functions **************************
void i2c_master_setup(void);
void i2c_master_start(void);
void i2c_master_restart(void);
void i2c_master_send(unsigned char byte);
unsigned char i2c_master_recv(void);
void i2c_master_ack(int val);
void i2c_master_stop(void);
void i2c_write(unsigned char address, unsigned char reg, unsigned char data);
char i2c_read(unsigned char address, unsigned char reg);

#endif