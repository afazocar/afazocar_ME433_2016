#include "pragmaConfig.h"   // PRAGMA Config file
#include <math.h>


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
    LATAbits.LATA4      = 0;        // Set A4 as low (off) to start
    
    __builtin_enable_interrupts();
        
    // ************************** I2C Functions **************************
    void i2c_master_setup(void) {
        //Change pins B2 and B3 from analog  
        ANSELBbits.ANSB2 = 0;
        ANSELBbits.ANSB3 = 0;

        I2C2BRG = 53;

        I2C2CONbits.ON = 1;               // Turn on I2C2 
    }

    void i2c_master_start(void) {
        I2C2CONbits.SEN = 1;            // Send start bit
        while(I2C2CONbits.SEN) {        // Wait for start bit 
            ; 
        }    
    }

    void i2c_master_restart(void) {     
        I2C2CONbits.RSEN = 1;           // Restart
        while(I2C2CONbits.RSEN) {       // Wait for restart to clear
            ; 
        }   
    }

    void i2c_master_send(unsigned char byte) {
        I2C2TRN = byte;                 // For address, bit 0: 0 for write, 1 for read
        while(I2C2STATbits.TRSTAT) {    // Wait for transmission to finish
            ; 
        }  
    }

    unsigned char i2c_master_recv(void) {
        I2C2CONbits.RCEN = 1;           // Receive data
        while(!I2C2STATbits.RBF) {      // Wait to receive
            ; 
        }    
        return I2C2RCV;
    }

    void i2c_master_ack(int val) {
        I2C2CONbits.ACKDT = val;        // Store acknowledge (0) or not (1))
        I2C2CONbits.ACKEN = 1;          // Send ACKDT
        while(I2C2CONbits.ACKEN) {      // Wait for acknowledge
            ; 
        }    
    }

    void i2c_master_stop(void) { 
        I2C2CONbits.PEN = 1;            // Comm is complete
        while(I2C2CONbits.PEN) {        // Wait for STOP to complete
            ; 
        }        
    }

    void i2c_write(unsigned char address, unsigned char reg, unsigned char data){ 
        i2c_master_start();             // Start bit

        i2c_master_send(address<<1|0);  // Write address, shift left by 1, OR with a 0 for writing

        i2c_master_send(reg);           // Register to write to

        i2c_master_send(data);          // Send data

        i2c_master_stop();              // Stop bit
    }

    char i2c_read(unsigned char address, unsigned char reg){ 
        i2c_master_start();             // Start bit

        i2c_master_send(address<<1|0);  // Write address, shift left by 1, OR with a 0 for writing

        i2c_master_send(reg);           // Register to read

        i2c_master_restart();           // Restart bit

        i2c_master_send(address<<1|1);  // Write address, shift left by 1, OR with a 1 for reading

        char r = i2c_master_recv();     // Value returned

        i2c_master_ack(1);              // Acknowledge received

        i2c_master_stop();              // Stop bit

        return r;
    }

    // ************************** I2C Setup **************************
    i2c_master_setup();     
    unsigned char I2C_setup; 
    
    // ************************** MAIN **************************
    
    // Check that WHO_AM_I returns 0b01101001
    char whoAmI = i2c_read(IMU, WHO_AM_I);
    if(whoAmI == 0b01101001)
    {
        LATAbits.LATA4 = 1; // Turn LED on
    }
    else
    {   
        LATAbits.LATA4 = 0; // Turn LED off
    }
    
    _CP0_SET_COUNT(0);
    while(1) {
    }
}