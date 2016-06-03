#include "pragmaConfig.h"   // PRAGMA Config file
//#include "spiSetup.h"
#include <math.h>

#define CS LATBbits.LATB7 // chip select pin
#define SineCount 100
#define TriangleCount 200
#define PI 3.14159265

static volatile float SineWaveform[SineCount];   // sine waveform
static volatile float TriangleWaveform[TriangleCount];   // triangle waveform
unsigned char read  = 0x00;
unsigned char checkGP7 = 0x00;
char SPI1_IO(char write);
void initSPI1();
void setVoltage(char channel, float voltage);
void initExpander();
void setExpander(int pin, int level);
char getExpander();
void makeSinWave();
void makeTriangleWave();
unsigned char setLowBitOperation(int pin);

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
    
    WDTCONbits.ON = 0;
    __builtin_enable_interrupts();
    
    /*// ************************** SPI Setup **************************
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
    }*/
        
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
      
    //Start with the LED on pin 0 off
    unsigned char data = 0b00000000;
    i2c_write(0b0100001, 0x0A, data);
    
    // ************************** MAIN **************************
    
    
    //int             Acount = 0, i = 0;
    //unsigned char   Alevel = 0, Blevel = 0;
    //unsigned char   sine[A_UPDATER];
    /*for (;i<A_UPDATER;i++)
    {
        sine[i] = ((LEVELS-1)/2*sin(((double)(2*PI*Acount))/((double)LEVELS)*2))+(LEVELS/2);
        Acount++;
    }
    Acount = 0;*/
    // PIC32 Setup

    //initSPI();
    
    makeSinWave();
    makeTriangleWave();
    initSPI1();
    i2c_master_setup();
    
    while(1) {
        _CP0_SET_COUNT(0);
        LATAINV = 0x10; // make sure timer2 works
        
        static int count1 = 0;
        static int count2 = 0;
//        setVoltage(0,SineWaveform[count1]);
        setVoltage(1,TriangleWaveform[count2]);
        count1++;
        count2++;
        if(count1 >= SineCount){
            count1 = 0;
        }
        if(count2 >= TriangleCount){
            count2 = 0;
        }
        while(_CP0_GET_COUNT() < 24000) {
            ;
        }
        
        //LATAbits.LATA4      = 1;        // Set A4 as high

        
        //setVoltage(B,100);
        //Blevel++;
        //while(_CP0_GET_COUNT()<(48000000/5/256/2)){;}
        
        // ************************** SPI **************************
        /*if(_CP0_GET_COUNT()>24000){ 
            i++;
            setVoltage(0, sinewave[i]);
            setVoltage(1, triangleWave[i]);
            _CP0_SET_COUNT(0);
         }
        if (i > 999){i = 0;}*/
        
        
        
        // ************************** I2C **************************
        char Button = i2c_read(0b0100001, 0x09);
        if((Button>>7)&0x01 == 1) // Button pressed
        {
            //Turn On LED
            unsigned char data = 0b00000000;
            i2c_write(0b0100001, 0x0A, data);
        }
        else
        {   
            //Turn Off LED
            unsigned char data = 0b00000001;
            i2c_write(0b0100001, 0x0A, data);
        }
    }
}

void initSPI1(){
    // set up the chip select pin as an output
    // the chip select pin is used by the MCP4902DAC to indicate
    // when a command is beginning (clear CS to low) and when it
    // is ending (set CS high)
    TRISBbits.TRISB7 = 0b0;
    CS = 1;
    SS1Rbits.SS1R = 0b0100;   // assign SS1 to RB7
    SDI1Rbits.SDI1R = 0b0000; // assign SDI1 to RA1
    RPB8Rbits.RPB8R = 0b0011; // assign SDO1 to RB8
    ANSELBbits.ANSB14 = 0;    // turn off AN10

    // setup SPI1
    SPI1CON = 0;              // turn off the SPI1 module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x1;            // baud rate to 12 MHz [SPI4BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.MODE32 = 0;   // use 8 bit mode
    SPI1CONbits.MODE16 = 0;
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on SPI 1
}

char SPI1_IO(char write){
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
     ;
     }
    return SPI1BUF;
}

void setVoltage(char channel, float voltage){
    int temp = voltage;
    if(channel == 0) { // 0 for VoutA
        CS = 0;
        SPI1_IO((temp >> 4) | 0b01110000); // 4 configuration bits
        SPI1_IO(temp << 4); // Data bits
        CS = 1;
    }
    if(channel == 1) { // 1 for VoutB
        CS = 0;
        SPI1_IO((temp >> 4) | 0b11110000); // 4 configuration bits
        SPI1_IO(temp << 4); // Data bits
        CS = 1;
    }
}

void makeSinWave(){
    int i;
    for(i = 0; i < SineCount; i++){
        SineWaveform[i] = 127+128*sin(2*PI*10*i*0.001);
        }
}

void makeTriangleWave(){
    int j;
    for(j = 0; j < TriangleCount; j++){
        TriangleWaveform[j] = 255*(j*0.005);
    }
}