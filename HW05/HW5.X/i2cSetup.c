#include "i2cSetup.h"

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