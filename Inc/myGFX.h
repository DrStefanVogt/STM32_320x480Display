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

void rectangle(uint16_t line, uint16_t row, uint16_t width, uint16_t height, uint16_t color);
void writeLetter(char letter, uint16_t x, uint16_t y,uint16_t color,uint16_t background);
void writeWord(const char *word, uint16_t x, uint16_t y,uint16_t color,uint16_t background);

#endif /* MYGFX_H_ */
