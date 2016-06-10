#include "tftLCD.h"
#include <xc.h>

#define CS LATBbits.LATB7       // Chip Select

void initSPI() {
	SDI1Rbits.SDI1R = 0b0100;   // B8 is SDI1
    RPA1Rbits.RPA1R = 0b0011;   // A1 is SDO1
    TRISBbits.TRISB7 = 0;       // CS is B7
    CS = 1;                     // CS starts high

    // A0 / DAT pin
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB15 = 0;
    LATBbits.LATB15 = 0;
	
	SPI1CON = 0;                // Reset SPI
    SPI1BUF;                    // Clear the Rx buffer
    SPI1BRG = 1;                // Baud rate
    SPI1STATbits.SPIROV = 0;    // Clear the overflow bit
    SPI1CONbits.CKE = 1;        // Data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;      // Master operation
    SPI1CONbits.ON = 1;         // Turn on SPI
}

unsigned char spiIO(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) {
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    LATBbits.LATB15 = 0;
    CS = 0;
    spiIO(com);
    CS = 1;
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB15 = 1;
    CS = 0;
    spiIO(dat);
    CS = 1;
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB15 = 1;
    CS = 0;
    spiIO(dat>>8);
    spiIO(dat);
    CS = 1;
}

void initLCD() {
    int time = 0;
    LCD_command(CMD_SWRESET);   //software reset
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 48000000/2/2) {} //delay(500);

	LCD_command(CMD_SLPOUT);    //exit sleep
    time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {} //delay(5);

	LCD_command(CMD_PIXFMT);    //Set Color Format 16bit
	LCD_data(0x05);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {} //delay(5);

	LCD_command(CMD_GAMMASET);  //default gamma curve 3
	LCD_data(0x04);             //0x04
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_GAMRSEL);   //Enable Gamma adj
	LCD_data(0x01);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_NORML);

	LCD_command(CMD_DFUNCTR);
	LCD_data(0b11111111);
	LCD_data(0b00000110);

    int i = 0;
	LCD_command(CMD_PGAMMAC);   //Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		LCD_data(pGammaSet[i]);
	}

	LCD_command(CMD_NGAMMAC);   //Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		LCD_data(nGammaSet[i]);
	}

	LCD_command(CMD_FRMCTR1);   //Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x08);             //0x0C//0x08
	LCD_data(0x02);             //0x14//0x08
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_DINVCTR);   //display inversion
	LCD_data(0x07);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_PWCTR1);    //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_data(0x0A);             //4.30 - 0x0A
	LCD_data(0x02);             //0x05
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_PWCTR2);    //Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_data(0x02);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_VCOMCTR1);  //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_data(0x50);             //0x50
	LCD_data(99);               //0x5b
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_VCOMOFFS);
	LCD_data(0);//0x40
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_CLMADRS);   //Set Column Address
	LCD_data16(0x00);
    LCD_data16(_GRAMWIDTH);

	LCD_command(CMD_PGEADRS);   //Set Page Address
	LCD_data16(0x00);
    LCD_data16(_GRAMHEIGH);

	LCD_command(CMD_VSCLLDEF);
	LCD_data16(0);              // __OFFSET
	LCD_data16(_GRAMHEIGH);     // _GRAMHEIGH - __OFFSET
	LCD_data16(0);

	LCD_command(CMD_MADCTL);    // rotation
    LCD_data(0b00001000);       // bit 3 0 for RGB, 1 for GBR, rotation: 0b00001000, 0b01101000, 0b11001000, 0b10101000

	LCD_command(CMD_DISPON);    //display ON
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}    //delay(1);

	LCD_command(CMD_RAMWR);     //Memory Write
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
    if (x>=0 && x<=127 && y>=0 && y<=127) {
        LCD_setAddr(x,y,x+1,y+1);
        LCD_data16(color);
    }
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
    LCD_command(CMD_CLMADRS);   // Column
    LCD_data16(x0);
	LCD_data16(x1);

	LCD_command(CMD_PGEADRS);   // Page
	LCD_data16(y0);
	LCD_data16(y1);

	LCD_command(CMD_RAMWR);     //Into RAM
}

void LCD_clearScreen(unsigned short color) {
    int i;
    LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
		for (i = 0;i < _GRAMSIZE; i++){
			LCD_data16(color);
		}
}

void LCD_drawChar(unsigned short x, unsigned short y, unsigned char c) {
    int ii = 0;
    for (ii=0; ii<5; ii++) {
        int jj = 0;
        for (jj=7; jj>=0; jj--) {
            if ((ASCII[c-0x20][ii] >> jj) & 0x01 == 1) LCD_drawPixel(x+ii,y+jj,MAGENTA);
            else LCD_drawPixel(x+ii,y+jj,BLACK);
        }
    }
}

void LCD_drawString(unsigned short x, unsigned short y, unsigned char *str) {
    int ii = 0;
    while(str[ii]) {
        LCD_drawChar(x+5*ii,y,str[ii]);
        ii++;
    }
}