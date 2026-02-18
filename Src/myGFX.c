/*
 * myGFX.c
 *
 *  Created on: Jan 12, 2026
 *      Author: stevo
 */

#include "myGFX.h"
#include "sbc_lcd01.h"

uint16_t localBuffer[8][8];
uint16_t singleColor = 0xFFFF;
uint16_t* singleBuffer = &singleColor;


typedef struct {
	uint16_t color;
	uint16_t bgColor;
	uint8_t thickness;
} graphicsOptions;

typedef struct {
	uint16_t color;
	uint16_t bgColor;
	uint16_t x;
	uint16_t y;
	uint8_t thickness;
	uint8_t xOffset;
	uint8_t height;
	uint8_t width;
	uint8_t frame;
	char digits_prev[MAXDIGIT+1];
	char digits[MAXDIGIT+1];
	uint8_t digitNo;

} LCDOptions ;

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
		['M'] = { .rows ={0xC3C3EFFF, 0xDBC3C3C3} },
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
		['!'] = { .rows ={0x60606060, 0x60006060} },
		['u'] = { .rows ={0x6C006C6C, 0x6C6C7C3C}},
		['$'] = { .rows ={0x30303C7C, 0x7C0C7070}},
		[','] = { .rows ={0x00000000, 0x00003070} },
};

// sine_table: 0..255 corresponds to sin(x)*255
static const uint8_t sine_table[] = {0,4,8,13,17,22,26,31,35,39,44,48,53,57,61,66,70,74,78,83,87,91,95,99,103,107,111,115,119,123,127,131,135,138,142,146,149,153,156,160,163,167,170,173,177,180,183,186,189,192,195,198,200,203,206,208,211,213,216,218,220,223,225,227,229,231,232,234,236,238,239,241,242,243,245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254,255,};

static textOptions TEXT_OPT ={1,COLOR16_BLACK,COLOR16_WHITE};
static graphicsOptions GRAPH_OPT ={COLOR16_BLACK,COLOR16_WHITE};
static LCDOptions LCD_OPT ={COLOR16_BLACK,COLOR16_WHITE,0,0,8,70,100,{'8','8','8','8','8'},{'8','8','8','8','8'},5}; //depends on MAXDIGIT!

void textInit(bool doubleSize, uint16_t color, uint16_t backgroundColor){
	TEXT_OPT.doubleSized = doubleSize;
	TEXT_OPT.color = color;
	TEXT_OPT.bgColor = backgroundColor;
}

void graphicsInit( uint16_t color, uint16_t backgroundColor, uint8_t thickness){
	//sets all graphics relecant options
	GRAPH_OPT.color = color;
	GRAPH_OPT.bgColor = backgroundColor;
	GRAPH_OPT.thickness = thickness;

}

void digitLCDInit(uint16_t x, uint16_t y, uint8_t xOffset, uint8_t height,uint8_t width, uint8_t digitNo){
	//initialize LCD display of digitNo numbers on screen, use graphicsInit first to set basic parameters
	LCD_OPT.color = GRAPH_OPT.color;
	LCD_OPT.bgColor = GRAPH_OPT.bgColor;
	LCD_OPT.x =x;
	LCD_OPT.y =y;
	LCD_OPT.thickness = GRAPH_OPT.thickness;
	LCD_OPT.xOffset = xOffset;
	LCD_OPT.digitNo = digitNo;
	LCD_OPT.height = height;
	LCD_OPT.width = width;
	LCD_OPT.frame = 10; //frame width in px
	//fill background
	rectangle(x-LCD_OPT.frame,y-LCD_OPT.frame,digitNo*xOffset+LCD_OPT.frame,height+2*LCD_OPT.frame, LCD_OPT.bgColor);
	for (uint8_t i=0;i < digitNo;i++){
		drawDigit_LCD(LCD_OPT.digits[i],x+(i*LCD_OPT.xOffset),y);

	}

}

void digitLCDUpdate(uint16_t input){
	convertUint16ToChar(input,LCD_OPT.digits,LCD_OPT.digitNo);

	for(uint8_t i=0;i< LCD_OPT.digitNo;i++){
		changeDigit_LCD(LCD_OPT.x+i*LCD_OPT.xOffset,LCD_OPT.y,i);
		drawDigit_LCD(LCD_OPT.digits[i],LCD_OPT.x+i*LCD_OPT.xOffset,LCD_OPT.y);
	}

}
void changeDigit_LCD(uint16_t x, uint16_t y, uint8_t i){
	uint8_t new = castCharToByte(LCD_OPT.digits[i]);
	uint8_t old = castCharToByte(LCD_OPT.digits_prev[i]);
	//if(old == new) return;
	uint8_t erase = (old & ~new);
	uint16_t color = LCD_OPT.color;
	LCD_OPT.color = LCD_OPT.bgColor;
	draw_statusByte(erase,x,y);
	LCD_OPT.color = color;
	LCD_OPT.digits_prev[i] = LCD_OPT.digits[i];
}

void setGraphicsColor(uint16_t color){
	//sets  graphics color
	GRAPH_OPT.color = color;
}

void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color){
	//draws a single color rectangle
	*singleBuffer=color;
	fillRectangle_oneColor(singleBuffer,x,y,width,height);
}

void rectangle_empty(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint8_t thickness, uint16_t color){
	//draws an empty rectangle without changing the inside pixels
	*singleBuffer=color;
	fillRectangle_oneColor(singleBuffer,x,y,width,thickness);
	fillRectangle_oneColor(singleBuffer,x,y+height,width+thickness,thickness);
	fillRectangle_oneColor(singleBuffer,x,y,thickness,width);
	fillRectangle_oneColor(singleBuffer,x+width,y,thickness, width);


}

void writeLetter(char letter, uint16_t x, uint16_t y,uint16_t color,uint16_t background){
	const Glyph *g = &font[(uint8_t)letter];
	uint8_t letterHeight = 8;
	uint8_t letterWidth = 8;
	initAdressWindow(x, y, letterWidth, letterHeight);
    for (uint8_t i = 0 ;i<4;i++){
    	for(uint8_t j=0; j<8;j++){
    			//fist half of letter
    			localBuffer[i][j] = (uint16_t)((g->rows[1]>>(i*8+j)) & 0x00000001) *color;
    			//second half of letter
    			localBuffer[4+i][j] = (uint16_t)((g->rows[0]>>(i*8+j)) & 0x00000001) *color;
    			//background
    			if(!((g->rows[0]>>(i*8+j)) & 0x00000001)) localBuffer[4+i][j]=background;
    			if(!((g->rows[1]>>(i*8+j)) & 0x00000001)) localBuffer[i][j]=background;
    	}
    }
	if (TEXT_OPT.doubleSized) fillSquare_scaleup(localBuffer,x,y,letterHeight);
	else fillRectangle(localBuffer,x,y,letterHeight,letterWidth);
}

void writeWord(const char *word, uint16_t x, uint16_t y){
	//assumes '\0' endend string
		uint8_t spacing;
		if (TEXT_OPT.doubleSized) spacing = 15;
		else spacing = 8;
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
			writeLetter((char)word[i],x-cursor,y,TEXT_OPT.color,TEXT_OPT.bgColor);

			if (i==MAXWORDLENGTH) break;
		}
}

int16_t sin_deg(int16_t x){
	//NOTE: -720<x<720 is required outside undefined
	// return values -255...+255
	int8_t sign=1;
	int16_t output=0;

	//check for negative x, fold back to [0,90) with right sign
	if (x<0) {
		sign = -sign;
		x =-x;
	}
	// accept 720° range,
	if(x>=360) x= x-360;

	//set output to correspondig value of sine_table
	// sine_table: 0..255 corresponds to sin(x)*255
	if (0 <= x && x <= 90) output = sine_table[x];
	if (90 < x && x <= 180) output = sine_table[-x+180];
	if (180 < x && x <= 270) output = -sine_table[x-180];
	if (270 < x && x <= 360) output = -(sine_table[-x+360]);
	return sign * output;
}

int16_t cos_deg(int16_t x){
	//NOTE: -720<x<720 is required outside undefined
	// return values -255...+255
	// accept 720° range
	if(x>=360) x= x-360;
	//NOTE: -360<x<360 is required
	return sin_deg(x-90);
}

void drawCircle(uint16_t x, uint16_t y, uint16_t d){
	for (uint16_t deg=0;deg <= 360; deg+=5){
		rectangle(x+sin_deg(deg)*d/255,y+cos_deg(deg)*d/255,GRAPH_OPT.thickness,GRAPH_OPT.thickness,GRAPH_OPT.color);
	}
	return;
}

void drawCircle_part(uint16_t x, uint16_t y, uint16_t d ,int16_t phi_start ,int16_t phi_stop){
	//phi=0 lowest point, draws clockwise, phi must be between -360 and +360, phi_start < phi_stop
	for (int16_t deg=phi_start;deg <= phi_stop; deg+=5){
		rectangle(x+sin_deg(deg)*d/255,y+cos_deg(deg)*d/255,GRAPH_OPT.thickness,GRAPH_OPT.thickness,GRAPH_OPT.color);
	}
	return;
}

void drawLine(uint16_t x, uint16_t y, uint16_t length, uint16_t phi){
	for (uint8_t step=0; step < length;step+=GRAPH_OPT.thickness>>1){
		rectangle(x+(step*cos_deg(phi)/255),y+(step*sin_deg(phi)/255),GRAPH_OPT.thickness,GRAPH_OPT.thickness,GRAPH_OPT.color);
	}
}

void drawUint16(uint16_t input, uint16_t x, uint16_t y, uint8_t max)
	{
	    char buffer[MAXDIGIT]={'0','0','0','0','0'};   // max "65535" + '\0'
	    convertUint16ToChar(input, buffer,max);
	    writeWord(buffer, x, y);
	}

void convertUint16ToChar(uint16_t input, char* buffer, uint8_t max){

    for(uint8_t i = 0;i<=max;i++) {
        buffer[i] = '0' + (input % 10);
        input /= 10;
    }
}

void drawDigit_LCD(char num, uint16_t x, uint16_t y){
	//draw single screen digit in digital clock style
	 uint8_t statusByte=castCharToByte(num);
	 draw_statusByte(statusByte,x,y);

}

void draw_statusByte(uint8_t statusByte, uint16_t x, uint16_t y){
	uint8_t thick = LCD_OPT.thickness;
	uint8_t width = LCD_OPT.width;
	uint8_t half_height = LCD_OPT.height>>1;
	 //	 _
	 // |_|
	 // |_x
	 // ----->
	 if(statusByte & (1U << 0)) rectangle(x,y,thick,half_height,LCD_OPT.color);
	 //	 _
	 // |_x
	 // |_|
	 // ----->
	 if(statusByte & (1U << 1)) rectangle(x,y+half_height,thick,half_height,LCD_OPT.color);
	 //	 x
	 // |_|
	 // |_|
	 //------>
	 if(statusByte & (1U << 2) )rectangle(x,y+2*half_height-thick,width+thick,thick,LCD_OPT.color);
	 //	 _
	 // x_|
	 // |_|
	 //------>
	 if(statusByte & (1U << 3)) rectangle(x+width,y+half_height,thick,half_height,LCD_OPT.color);

	//	_
	// |_|
	// x_|
	//----->
	 if(statusByte & (1U << 4)) rectangle(x+width,y,thick,half_height+thick/2,LCD_OPT.color);

	 //	 _
	 // |_|
	 // |x|
	 //------>
	 if(statusByte & (1U << 5)) rectangle(x,y,width+thick,thick,LCD_OPT.color);
	 //	 _
	 // |x|
	 // |_|
	 //----->
	 if(statusByte & (1U << 6)) rectangle(x,y+half_height-thick/2,width+thick,thick,LCD_OPT.color);
	 return;

}
void eraseDigit_LCD(char num, uint16_t x, uint16_t y){
	//erase number with background color
	uint16_t color = LCD_OPT.color;
	LCD_OPT.color = LCD_OPT.bgColor;
	drawDigit_LCD(num, x, y);
	LCD_OPT.color = color;
	return;
}

char castInt8ToChar(uint8_t input){
    char buffer='0';   // max "65535" + '\0'
    buffer= '0' + (input % 10);
    return buffer;

}

uint8_t castCharToByte(char num){

	 uint8_t statusByte=0b11110111;
	 switch (num){
	 	case '0': statusByte = 0b00111111;break;
	 	case '1': statusByte = 0b00000011;break;
	 	case '2': statusByte = 0b01110110;break;
	 	case '3': statusByte = 0b01100111;break;
	 	case '4': statusByte = 0b01001011;break;
	    case '5': statusByte = 0b01101101;break;
	    case '6': statusByte = 0b01111101;break;
	   	case '7': statusByte = 0b00000111;break;
	   	case '8': statusByte = 0b01111111;break;
	   	case '9': statusByte = 0b01101111;break;
	 }
	 return statusByte;
}
//****************************************************************************************************
//							debug functions
//****************************************************************************************************

void debugSmilie(void){
	//just a smilie using circles - see if it looks ok
		graphicsInit(COLOR16_BLUE,COLOR16_WHITE,5);
		for (uint8_t d=5;d<30;d+=17)	drawCircle(150+10, 100+d/2, d);
		for (uint8_t d=5;d<30;d+=17)	drawCircle(120-10, 100+d/2, d);
		drawCircle_part(132, 40, 26,-90 ,90);
		setGraphicsColor(COLOR16_RED);
		drawCircle(132,63,10);
}

void debugGrid(void){
	//draws a grid to simplify finding display locations. Big dots: 100 , small dots:10
	for(uint16_t x=0;x <DISPLAY_X_MAX; x+=10){
		for(uint16_t y=0 ;y <DISPLAY_Y_MAX; y+=10){
			if(x%100 ==0 || y%100 ==0 ) rectangle(x,y,5,5,COLOR16_BLACK);
			else rectangle(x,y,1,2,COLOR16_BLACK);
		}
		}
	writeLetter('O',0,0,COLOR16_RED,COLOR16_BLACK);
	}

void debugSimpleCounter(void){
	graphicsInit( COLOR16_BLACK,COLOR16_WHITE, 5);

	for(uint8_t n = 0; n<10;n++) {
		drawDigit_LCD(castInt8ToChar(n),140,100);
		systick_msec_sleep(150);
		eraseDigit_LCD(castInt8ToChar(n),140,100);
}
}

void debugSineCosine(void){
uint32_t x = 0;
uint32_t y =240;

fullScreenColor(COLOR16_WHITE);
for (uint16_t i=0;i<DISPLAY_X_MAX;i+=1){
	rectangle(x+i,y+105+((sin_deg(i*2))>>2),3,3,COLOR16_BLUE);
	rectangle(x+i,y+105+((cos_deg(i*2))>>2),3,3,COLOR16_LIGHTBLUE);
}
graphicsInit(COLOR16_RED,COLOR16_WHITE,5);

for (uint16_t phi=0;phi<=360;phi+=15){
		drawLine(x+50,y+195,20,phi);
	}
graphicsInit(COLOR16_BLACK,COLOR16_WHITE,5);
drawLine(x+178,y+220,46,0);
drawLine(x+155,y+220,62,270);
drawCircle_part(155,425,30,-90,90);
textInit(1, COLOR16_BLACK, COLOR16_WHITE);
writeWord("STEFANS ANKERALARM",x+300,y+10);
	}
