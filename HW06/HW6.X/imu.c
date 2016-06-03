#include "imu.h"
#include "i2c.h"

unsigned char chip = 0b1101011;

void initIMU() {
    i2c_master_start();
    i2c_master_send((chip << 1) | 0x00);
    i2c_master_send(0b00010000);
    i2c_master_send(0b10000000);
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send((chip << 1) | 0x00); 
    i2c_master_send(0b00010001);
    i2c_master_send(0b10000000);
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send((chip << 1) | 0x00);
    i2c_master_send(0b00010010);
    i2c_master_send(0b00000100);
    i2c_master_stop();
}

void readIMU(char regstr, unsigned char* data, char length) {
    i2c_read(chip, regstr, data, length);
}