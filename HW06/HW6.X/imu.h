#ifndef _IMU_H
#define _IMU_H

void initIMU();
void readIMU(char regstr, unsigned char* data, char length);

#endif