#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spiSetup.h"
#include "I2C.h"
#include <math.h>

// DEVCFG0
#pragma config DEBUG = 0x1 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // free up secondary osc pins
#pragma config FPBDIV = 00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 0x1 // do not enable clock switch
#pragma config WDTPS = 00000 // slowest wdt
#pragma config WINDIS = 1 // no wdt window
#pragma config FWDTEN = 0 // wdt off by default
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 111 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiply
    //by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0xbebe // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


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
    
    initSPI1(); // initialize spi
    initI2C(); // initialize i2c
    initExpander; // initialize I/O expander
    
    double Vouta = 128; // sin wave 10 Hz
    double Voutb = 0; // triangle wave 5 Hz
    double time = 0; // time to calculate sine
    
    while(1) {
        
        _CP0_SET_COUNT(0);  // set system clock to zero
        while(_CP0_GET_COUNT() < 24000) { // wait 1ms
        ; }
        
        setVoltage(0,Vouta);
        setVoltage(1,Voutb);
        
        // update voltage values
        Vouta = 128 + 127*sin(62.831*time); // 2*pi*10Hz = 62.831
        time = time + 0.001;
        Voutb = Voutb + 1.28; // (256/200) time steps
        if(Voutb > 256) {
            Voutb = 0; // reset at end of triangle
        }
        
        char button = getExpander();
        if((button & 0x80) >> 7 != 0) // user pressing button
        {
            setExpander(0, 1);
        } else {
            setExpander(0, 0);
        }
       
    }
    
}