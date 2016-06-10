#ifndef I2C_SETUP_H
#define I2C_SETUP_H
#define SLAVE_ADDR (char) 0x20

char            i2cMasterRead(char reg_addr);
void            i2cMasterWrite(char reg_addr, char write);
void            i2cMasterSetup(void);              
void            i2cMasterStart(void);              
void            i2cMasterRestart(void);            
void            i2cMasterSend(unsigned char byte); 
unsigned char   i2cMasterRecv(void);                
void            i2cMasterAck(int val);             
void            i2cMasterStop(void);               

#endif