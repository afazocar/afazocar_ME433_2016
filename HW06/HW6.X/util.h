#ifndef UTIL_H
#define UTIL_H
#include <xc.h>
#define PIC_FREQ    48000000
#define SAM_FREQ    50
#define PER2        2999
#define UPDATER     PIC_FREQ/SAM_FREQ
void char2short(char *data, short *output, int sizedata);
void tim2_setup(void);
void oc1_setup(void);
void oc2_setup(void);
#endif