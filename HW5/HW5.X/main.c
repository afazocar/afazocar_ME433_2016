#include "pragmaConfig.h"   // PRAGMA Config file
#include "i2cSetup.h"       // I2C functions
#include "tftLCD.h"         // LCD functions
#include <math.h>
#include <xc.h>           
#include <sys/attribs.h>

#define IMU         0b1101010   // IMU address (page 29)
#define WHO_AM_I    0x0F        // WHO_AM_I address (page 37)
#define CTRL1_XL    0x10        // Acceleromter address (page 37)
#define CTRL2_G     0x11        // Gyro address (page 37)

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
    
    // do your TRIS and LAT commands here 
    TRISBbits.TRISB4    = 1;        // Set B4 (push-button) as input pin
    TRISAbits.TRISA4    = 0;        // Set A4 (LED) as output
    LATAbits.LATA4      = 1;        // Set A4 as hi (on) to start
    
    __builtin_enable_interrupts();
        
    initSPI();
    initLCD();
    
    i2c_master_setup();     
    unsigned char I2C_setup; 
         
    LCD_clearScreen(BLACK);
    char string[100];
    int num = 1337;
    sprintf(string, "Hello World %d!", num);
    LCD_drawString(28,32,string);
    
    /*// Check that WHO_AM_I returns 0b01101001
    char whoAmI = i2c_read(IMU, WHO_AM_I);
    if(whoAmI == 0b01101000)
    {
        sprintf(string, "WHO_AM_I = GOOD");
        LCD_drawString(28,42,string);
    }
    else
    {   
        sprintf(string, "WHO_AM_I = BAD");
        LCD_drawString(28,42,string);
    }*/
    
    _CP0_SET_COUNT(0);
    while(1) {
        
        
    }
}