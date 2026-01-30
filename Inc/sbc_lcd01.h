/*
 * sbc_lcd01.h
 *
 *  Created on: Jan 3, 2026
 *      Author: stevo
 */
#include "spi.h"
#include "spi_dma.h"
#include "gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "systick.h"
#include "exti.h"

#ifndef SBC_LCD01_H_
#define SBC_LCD01_H_


/*from adafruit_ST7789.cpp*/
#define ST77XX_SWRESET 0x01
#define ST_CMD_DELAY 0x80 // special signifier for command lists
#define ST77XX_SLPOUT 0x11
#define ST77XX_COLMOD 0x3A
#define ST77XX_MADCTL 0x36
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_INVON 0x21
#define ST77XX_NORON 0x13
#define ST77XX_DISPON 0x29
#define ST7789_DISPON 0x29
#define ST77XX_RAMWR 0x2C
#define ST7789_DISPOFF 0x28


#define DISPLAY_X_MAX 240 //for 16 bit color_transfer
#define DISPLAY_Y_MAX 241
#define DISPLAY_PIXEL (DISPLAY_X_MAX * DISPLAY_Y_MAX)
#define MAX_WINDOW_PIXEL (DISPLAY_X_MAX*16)

#define COLOR16_WHITE 0xFFFF
#define COLOR16_BLACK 0x0000
#define COLOR16_RED 0xF800
#define COLOR16_BLUE 0x00FD
#define COLOR16_GREEN 0x1F00
#define COLOR16_LIGHTBLUE 0xAAFF
#define MAXDIGIT 5
#define MAXWORDLENGTH 25

static volatile uint16_t windowBuffer[DISPLAY_X_MAX*2]; // define windowBuffer for


/*from adafruit_ST7789.cpp*/
static const uint8_t generic_st7789[] ={                // Init commands for 7789 screens
	    9,                              //  9 commands in list:
	    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
	      150,                          //     ~150 ms delay
	    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
	      10,                          //      10 ms delay
	    ST77XX_COLMOD , 1+ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
	      0x55,                         //     16-bit color
	      10,                           //     10 ms delay
	    ST77XX_MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
	      0x00,                         //     Row/col addr, bottom-top refresh
	    ST77XX_CASET  , 4,              //  5: Column addr set, 4 args, no delay:
	      0x00,
	      0,        //     XSTART = 0
	      0,
	      240,  //     XEND = 240
	    ST77XX_RASET  , 4,              //  6: Row addr set, 4 args, no delay:
	      0x00,
	      0,             //     YSTART = 0
	      239>>8,
	      239&0xFF,  //     YEND =239
	    ST77XX_INVON  ,   ST_CMD_DELAY,  //  7: hack
	      10,
	    ST77XX_NORON  ,   ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
	      10,                           //     10 ms delay
	    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
	      10 };


void cs_enable(void);
void cs_disable(void);
void tft_dc_low(void);
void tft_dc_high(void);
void displayInit(const uint8_t *addr);
void displayReset(void);
void sendCommand(uint8_t commandByte, const uint8_t *dataBytes, uint8_t numDataBytes);
void initAdressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void sbc_lcd01_init();
void fullScreenColor(uint16_t color);
void testScreen_16(void);
void fillRectangle(uint16_t *buffer,int16_t x,int16_t y, uint8_t a, uint8_t b);
void fillSquare_scaleup(uint16_t (*buffer)[8], uint16_t x, uint16_t y, uint16_t a);
void fillRectangle_oneColor(uint16_t *buffer,int16_t x,int16_t y, uint8_t a, uint8_t b);
void setsingleColorStatus(bool singleColor);
#endif /* SBC_LCD01_H_ */
