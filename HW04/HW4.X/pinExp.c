#include "pinExp.h"

void initExp(char IO_dir)
{
    ANSELBbits.ANSB2 = 0;       // I2C2 analog off;
    ANSELBbits.ANSB3 = 0;       // I2C2 analog off;
    i2cMasterSetup();
    i2cMasterWrite(IODIR,IO_dir);
}
void setExp(char pin, char level)
{
    char read_byte,write_byte;
    read_byte = i2cMasterRead(OLAT);
    if (level==0)
    {
        write_byte = read_byte&~(pin);
    }
    else if (level==1)
    {
        write_byte = read_byte|pin;
    }
    i2cMasterWrite(OLAT,write_byte);
}
char getExp()
{
    return i2cMasterRead(GPIO);
}