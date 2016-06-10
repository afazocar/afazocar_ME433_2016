#include "pragmaConfig.h"
#include <stdio.h>

int main()
{
    char            whoamiCheck = 0;
    unsigned char   data[14];
    short           output[7];
    int             oc1New      = 0;
    int             oc2New      = 0;
    
    // PIC32 Setup
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;      // 0 data RAM access wait states
    INTCONbits.MVEC     = 0x1;      // enable multi vector interrupts
    DDPCONbits.JTAGEN   = 0;        // disable JTAG to get pins back
    TRISAbits.TRISA4    = 0;        // RA4 output
    TRISBbits.TRISB4    = 1;        // RB4 input
    LATAbits.LATA4      = 0;		// LED off
    
    i2cMasterSetup();
    tim2Setup();
    oc1Setup();
    oc2Setup();
    
    __builtin_enable_interrupts();
    
    whoamiCheck = i2cMasterRead(IMU_ADDR,WHO_AM_I);
    if (whoamiCheck == WHOAMI_VAL)
    {
        LATAbits.LATA4 = 1;
    }
    
    while(1)
    {
        _CP0_SET_COUNT(0);
        i2cMasterReadAll(IMU_ADDR,OUT_TEMP_L,14,data);
        char2short(data,output,14);
        oc1New = (PER2+1)/2+(output[4]/20);
        oc2New = (PER2+1)/2+(output[5]/20);
        if(oc1New>(PER2+1)){
            oc1New=(PER2+1);
        }
        if(oc2New>(PER2+1)){
            oc2New=(PER2+1);
        }
        OC1RS = oc1New;
        OC2RS = oc2New;
        LATAbits.LATA4 = !LATAbits.LATA4;
        while(_CP0_GET_COUNT()<UPDATER){;}
    }   
}