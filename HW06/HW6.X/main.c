#include "pragmaConfig.h"   // PRAGMA Config file
#include "i2c.h"
#include "imu.h"
#include <math.h>

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    TRISBbits.TRISB4 = 1;
    
    __builtin_enable_interrupts();
    
    //PWM initialization
    //RPB9Rbits.RPB9R = 0b0101; 
    RPA0Rbits.RPA0R = 0b0101; // OC1
    RPA1Rbits.RPA1R = 0b0101; // OC2
    T2CONbits.TCKPS = 0b100;
	PR2 = 2999;//6000; 
	TMR2 = 0; 
    T2CONbits.ON = 1;
    
    OC1CONbits.OCM = 0b110;
	//OC1CONbits.OC32 = 0; 
	OC1CONbits.OCTSEL = 0; 
	//OC1RS = 1500*3.4;//3200;
	//OC1R = 1500*3.4;//3200;
    OC1CONbits.ON = 1;
    
    OC2CONbits.OCM = 0b110;
	//OC2CONbits.OC32 = 0; 
	OC2CONbits.OCTSEL = 0; 
	//OC2RS = 1500*3.4;//3200;
	//OC2R = 1500*3.4;//3200;
    OC2CONbits.ON = 1;
    
    initI2C(); // initialize i2c
    initIMU(); // initialize IMU
    
    static unsigned char data[14]; 
    static short short_data[7];
    
    int i=0;
    for(i=0; i<14; i++) {
        data[i] = 0x00;
    }
    for(i=0; i<7; i++) {
        short_data[i] = 0x0000;
    }
    
    while(1) {
        
        _CP0_SET_COUNT(0);  // set system clock to zero
        
        
        readIMU(0b00100000, data, 14);
        int i;
        for(i=0; i<7; i++) {
            short_data[i] = ((short)data[2*i+1] << 8 | (short)data[2*i]);
        }
        
        OC1RS = 1500 + short_data[4]/20;//(int)((((float)short_data[4]*2 + 32767.0)/65535.0)*(6000));
        OC2RS = 1500;//(int)((((float)short_data[5]*2 + 32767.0)/65535.0)*(6000)); 
       
        while(_CP0_GET_COUNT() < 960000) { // wait 1ms
        ; }
    }
    
}