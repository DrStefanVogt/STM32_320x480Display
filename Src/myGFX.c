/*
 * myGFX.c
 *
 *  Created on: Jan 12, 2026
 *      Author: stevo
 */

#include "myGFX.h"
#include "sbc_lcd01.h"

uint16_t localBuffer[64];

const uint8_t MAXWORDLENGTH = 25;

typedef struct {
	bool doubleSized;
	uint16_t color;
	uint16_t bgColor;
} textOptions;

typedef struct {
	uint32_t rows[2];
} Glyph;

static const Glyph font[] = {
		['A'] = { .rows ={0x3C3C667E, 0x7E666666} },
		['B'] = { .rows ={0x7C7C667E, 0x7C667E7C} },
		['C'] = { .rows ={0x3C3C6666, 0x60667E3C} },
		['D'] = { .rows ={0x7C7C6666, 0x66667E7C} },
		['E'] = { .rows ={0x7E7E607C, 0x7C607E7E} },
		['F'] = { .rows ={0x7E7E607C, 0x7C606060} },
		['G'] = { .rows ={0x3E3E606E, 0x6E667E3E} },
		['H'] = { .rows ={0x6666667E, 0x7E666666} },
		['I'] = { .rows ={0x60606060, 0x60606060} },
		['J'] = { .rows ={0x0C0C0C0C, 0x6C6C3030} },
		['K'] = { .rows ={0x66666C7C, 0x706C6E66} },
		['L'] = { .rows ={0x60606060, 0x60607C7C} },
		['M'] = { .rows ={0x6161777F, 0x6D616161} },
		['N'] = { .rows ={0x6666767E, 0x6E666666} },
		['O'] = { .rows ={0x3C3C6666, 0x66667E3C} },
		['P'] = { .rows ={0x7C7C667E, 0x7C606060} },
		['Q'] = { .rows ={0x3C3C6666, 0x666C7E36} },
		['R'] = { .rows ={0x7C7C667E, 0x7C666666} },
		['S'] = { .rows ={0x3E3E607C, 0x3C067E7C} },
		['T'] = { .rows ={0x7C7C3030, 0x30303030} },
		['U'] = { .rows ={0x66666666, 0x66663C3C} },
		['V'] = { .rows ={0x6C6C6C6C, 0x6C303030} },
		['W'] = { .rows ={0x6D6D6D6D, 0x6D363636} },
		['X'] = { .rows ={0x6C6C6C30, 0x306C6C6C} },
		['Y'] = { .rows ={0x6C6C6C30, 0x30303030} },
		['Z'] = { .rows ={0x7C7C0C3C, 0x30607C7C} },
		['0'] = { .rows ={0x30306C6C, 0x6C6C7C30} },
		['1'] = { .rows ={0x30307070, 0x30303030} },
		['2'] = { .rows ={0x70700C3C, 0x30607C7C} },
		['3'] = { .rows ={0x70700C3C, 0x300C7C70} },
		['4'] = { .rows ={0x0C0C3C7C, 0x6C7C7C0C} },
		['5'] = { .rows ={0x7C7C6070, 0x700C7C70} },
		['6'] = { .rows ={0x3C3C6070, 0x706C7C30} },
		['7'] = { .rows ={0x7C7C0C30, 0x30303030} },
		['8'] = { .rows ={0x30306C7C, 0x306C7C30} },
		['9'] = { .rows ={0x30306C7C, 0x3C0C7C70} },
};

static textOptions TEXT_OPT ={1,COLOR16_BLACK,COLOR16_WHITE};

void textInit(bool doubleSize, uint16_t color, uint16_t backgroundColor){
	TEXT_OPT.doubleSized = doubleSize;
	TEXT_OPT.color = color;
	TEXT_OPT.bgColor = backgroundColor;
}


void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color){
	uint16_t pixelNo = width*height;
	/*for (uint16_t i=0; i < pixelNo ;i++){*/
		localBuffer[0]=color;
	//}
	fillRectangle_oneColor(localBuffer,x,y,width,height);
}

void writeLetter(char letter, uint16_t x, uint16_t y,uint16_t color,uint16_t background){
	const Glyph *g = &font[(uint8_t)letter];
	uint8_t letterHeight = 8;
	uint8_t letterWidth = 8;
	initAdressWindow(x, y, letterHeight, letterWidth);
    for (int8_t i = 31 ;i>=0;i--){
    	localBuffer[i] = (uint16_t)(g->rows[1]>>i & 0x00000001) *color;
    	localBuffer[32+i] = (uint16_t)(g->rows[0]>>i & 0x00000001) *color;
    	if(!(g->rows[1]>>i & 0x00000001)) localBuffer[i]=background;
    	if(!(g->rows[0]>>i & 0x00000001)) localBuffer[32+i]=background;
    }
	if (TEXT_OPT.doubleSized) fillSquare_scaleup(localBuffer,x,y,letterHeight);
	else fillRectangle(localBuffer,x,y,letterHeight,letterWidth);
}

void writeWord(const char *word, uint16_t x, uint16_t y,uint16_t color){
		uint8_t spacing = 15;
		int16_t cursor = -spacing;
		for(uint8_t i=0 ; word[i] !='\0'; i++){
			switch(word[i-1]){
			case('I'):
				cursor += (spacing-8);
				break;
			case('L'):
					cursor += (spacing-2);
					break;
			default:
			cursor += spacing;
			}
			writeLetter((char)word[i],x-cursor,y,color,TEXT_OPT.bgColor);

			if (i==MAXWORDLENGTH) break;
		}
}

