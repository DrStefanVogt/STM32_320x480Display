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


#define DISPLAY_X_MAX 320 //for 16 bit color_transfer
#define DISPLAY_Y_MAX 480
#define DISPLAY_PIXEL (DISPLAY_X_MAX * DISPLAY_Y_MAX)
#define MAX_WINDOW_PIXEL (DISPLAY_X_MAX*16)

#define COLOR16_WHITE 0xFFFF
#define COLOR16_BLACK 0x0000
#define COLOR16_RED 0xF800
#define COLOR16_BLUE 0x00FD
#define COLOR16_GREEN 0x1F00
#define COLOR16_LIGHTBLUE 0x867F
#define MAXDIGIT 5
#define MAXWORDLENGTH 50

static volatile uint16_t windowBuffer[DISPLAY_X_MAX*2]; // define windowBuffer for



/* FOR 7796: Remember to Set CPOL to 0 and CPHA to 0 in spi.c
if(0){SPI1->CR1 |=(1U<<0);}else{SPI1->CR1 &=~(1U<<0);}
if(0){SPI1->CR1 |=(1U<<1);}else{SPI1->CR1 &=~(1U<<1);}
*/
static const uint8_t st7796s_init[] = {14, // 14 commands
                                               ST77XX_SWRESET,
                                               ST_CMD_DELAY, // Software reset
                                               150,
                                               0xF0,
                                               1, // Unlock manufacturer
                                               0xC3,
                                               0xF0,
                                               1,
                                               0x96,
                                               0xC5,
                                               1, // VCOM Control
                                               0x1C,
                                               ST77XX_MADCTL,
                                               1, // Memory Access
                                               0x48,
                                               ST77XX_COLMOD,
                                               1, // Color Mode - 16 bit
                                               0x55,
                                               0xB0,
                                               1, // Interface Control
                                               0x80,
                                               0xB4,
                                               1, // Inversion Control
                                               0x00,
                                               0xB6,
                                               3, // Display Function Control
                                               0x80,
                                               0x02,
                                               0x3B,
                                               0xB7,
                                               1, // Entry Mode
                                               0xC6,
                                               0xF0,
                                               1, // Lock manufacturer commands
                                               0x69,
                                               0xF0,
                                               1,
                                               0x3C,
                                               ST77XX_SLPOUT,
                                               ST_CMD_DELAY, // Exit sleep
                                               150,
                                               ST77XX_DISPON,
                                               ST_CMD_DELAY, // Display on
                                               150};


void four_inch_init(void);
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
void setSingleColorStatus(bool singleColor);
#endif /* SBC_LCD01_H_ */
