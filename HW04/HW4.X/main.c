#include "pragmaConfig.h"   // PRAGMA Config file
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
    
    // do your TRIS and LAT commands here 
    TRISBbits.TRISB4    = 1;        // Set B4 (push-button) as input pin
    TRISAbits.TRISA4    = 0;        // Set A4 (LED) as output
    LATAbits.LATA4      = 1;        // Set A4 as high
    
    __builtin_enable_interrupts();
    
    
    // ************************** SPI Functions **************************
    unsigned char spiIO(unsigned char x) {
      SPI1BUF = x;
      while(!SPI1STATbits.SPIRBF) {     // Wait for byte
        ;
      }
      return SPI1BUF;
    }

    void initSPI(){
        RPB8Rbits.RPB8R = 0b0011;   // Set RPB8 to SDO1
        TRISBbits.TRISB7 = 0;       // Set the CS pin to be a digital output
        LATBbits.LATB7 = 1;         // Start CS pin hi

        SPI1CON = 0;                // Reset SPI
        SPI1BUF;                    // Clear the Rx buffer
        SPI1BRG = 11999;            // Baud rate to 10 MHz
        SPI1STATbits.SPIROV = 0;    // Clear the overflow bit
        SPI1CONbits.CKE = 1;        // Data changes when clock goes from hi to lo (since CKP is 0)
        SPI1CONbits.MSTEN = 1;      // Master operation
        SPI1CONbits.ON = 1;         // Turn on SPI
    }

    void setVoltage(char channel, unsigned char voltage){
        unsigned short buffer = 0x0;
        buffer = buffer | channel << 15;
        buffer = buffer | 0x7 << 12;
        buffer = buffer | voltage << 4;

        LATBbits.LATB7 = 0;             // Set CS lo
        spiIO((buffer & 0xFF00) >> 8);
        spiIO(buffer & 0x00FF);
        LATBbits.LATB7 = 1;             // Set CS hi
    }
    
    // ************************** SPI Setup **************************
    initSPI();
    unsigned char initVolts = 0;
    char output = 0;    
    setVoltage(output, initVolts);
    
    // Create waves
    unsigned char sinewave[1000];
    unsigned char triangleWave[1000];
    int i = 0;
    int center = 255/2;
    int A = 255/2;
    
    for (i = 0; i < 1000; i++){
        sinewave[i]     = (unsigned char)center + A*sin(2*3.14*i/1000);
        triangleWave[i] = (unsigned char)255*i/1000;
    }
        
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
    
    I2C_setup = 0xF0;                       // Pins 0-3 as outputs and 4-7 as inputs
    i2c_write(0b0100001, 0x00, I2C_setup);
      
    //Start with the LED on pin 0 on
    unsigned char data = 0b00000001;
    i2c_write(0b0100001, 0x0A, data);
    
    // ************************** MAIN **************************
    _CP0_SET_COUNT(0);
    while(1) {
        
        // ************************** SPI **************************
        if(_CP0_GET_COUNT()>24000){ 
            i++;
            setVoltage(0, sinewave[i]);
            setVoltage(1, triangleWave[i]);
            _CP0_SET_COUNT(0);
         }
        if (i > 999){i = 0;}
        
        // ************************** I2C **************************
        char Button = i2c_read(0b0100001, 0x09);
        if((Button>>7)&0x01 == 1) // Button pressed
        {
            //Turn Off LED
            unsigned char data = 0b00000000;
            i2c_write(0b0100001, 0x0A, data);
        }
        else
        {   
            //Turn on LED
            unsigned char data = 0b00000001;
            i2c_write(0b0100001, 0x0A, data);
        }
    }
}