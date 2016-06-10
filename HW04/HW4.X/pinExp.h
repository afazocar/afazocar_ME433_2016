#ifndef PIN_EXP_H
#define PIN_EXP_H
#include "i2cSetup.h"
#include <xc.h>

// MCP23008 ADDRESSES AND VALUES
#define IODIR   (char) 0x00
#define GPIO    (char) 0x09
#define OLAT    (char) 0x0A
#define GP0     (char) 1<<0
#define GP1     (char) 1<<1
#define GP2     (char) 1<<2
#define GP3     (char) 1<<3
#define GP4     (char) 1<<4
#define GP5     (char) 1<<5
#define GP6     (char) 1<<6
#define GP7     (char) 1<<7

void initExp(char IO_dir);
void setExp(char pin, char level);
char getExp();
#endif