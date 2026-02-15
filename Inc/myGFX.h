/*
 * myGFX.h
 *
 *  Created on: Jan 12, 2026
 *      Author: stevo
 */
#include <stdint.h>
#include "sbc_lcd01.h"
#ifndef MYGFX_H_
#define MYGFX_H_


void textInit(bool doubleSize, uint16_t color, uint16_t backgroundColor);
void digitLCDInit(uint16_t x, uint16_t y, uint8_t xOffset, uint8_t height,uint8_t width, uint8_t digitNo);
void graphicsInit( uint16_t color, uint16_t backgroundColor, uint8_t thickness);
void digitLCDUpdate(uint16_t input);

void rectangle(uint16_t line, uint16_t row, uint16_t width, uint16_t height, uint16_t color);
void rectangle_empty(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint8_t thickness, uint16_t color);
void writeLetter(char letter, uint16_t x, uint16_t y,uint16_t color,uint16_t background);
void writeWord(const char *word, uint16_t x, uint16_t y);
char castInt8ToChar(uint8_t input);
void convertUint16ToChar(uint16_t input, char* buffer, uint8_t max);
void drawUint16(uint16_t input, uint16_t x, uint16_t y, uint8_t max);
void drawLine(uint16_t x, uint16_t y, uint16_t length, uint16_t phi);
void drawCircle_part(uint16_t x, uint16_t y, uint16_t d ,int16_t phi_start ,int16_t phi_stop);
void drawCircle(uint16_t x, uint16_t y, uint16_t d);
void drawDigit_LCD(char num, uint16_t x, uint16_t y);
void eraseDigit_LCD(char num, uint16_t x, uint16_t y);
void changeDigit_LCD(uint16_t x, uint16_t y, uint8_t i);
void draw_statusByte(uint8_t statusByte, uint16_t x, uint16_t y);
uint8_t castCharToByte(char num);
int16_t cos_deg(int16_t x);
int16_t sin_deg(int16_t x);

void debugSimpleCounter (void);
void debugGrid(void);
void debugSmilie(void);
void debugSineCosine(void);
#endif /* MYGFX_H_ */
