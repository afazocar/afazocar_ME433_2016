#ifndef I2C_SETUP_H
#define I2C_SETUP_H
#define     CTRL1_XL    (char)  0x10    //  0b00010000
#define     CTRL2_G     (char)  0x11    //  0b00010001
#define     CTRL3_C     (char)  0x12    //  0b00010010
#define     CTRL1_VAL   (char)  0x80    //  0b10000000
#define     CTRL2_VAL   (char)  0x80    //  0b10000000
#define     CTRL3_VAL   (char)  0x04    //  0b00000100
#define     OUT_TEMP_L  (char)  0x20    //  0b00100000
#define     WHO_AM_I    (char)  0x0F    //  0b00001111
#define     WHOAMI_VAL  (char)  0x69    //  0b01101001
#define     IMU_ADDR    (char)  0x6B    //  0b1101011x(R/W)

char            i2cMasterRead(char device, char reg_addr);
void            i2cMasterReadAll(char device,char start_reg,int numval,unsigned char* array);
void            i2cMasterWrite(char device, char reg_addr, char byte);
void            i2cMasterSetup(void);  
void            i2cMasterStart(void);  
void            i2cMasterRestart(void);
void            i2cMasterSend(unsigned char byte);
unsigned char   i2cMasterRecv(void);  
void            i2cMasterAck(int val);
void            i2cMasterStop(void); 

#endif