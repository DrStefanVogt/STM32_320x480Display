/*
 * sbc_lcd01.c
 *
 *  Created on: Jan 3, 2026
 *      Author: stevo
 */
#include "spi.h"
#include "sbc_lcd01.h"
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
         //    10 ms delay

static uint16_t lineBuffer[DISPLAY_X_MAX];
static bool singleColorStatus;

/*3 basic ways to transmit data
 * sendCommand = simplest form of 8 bit command used for display_init and initAdressWindow
 * sendCommand16 = 16 bit single command used for all other functions to send commands to display before DMA data is transfered
 * spi3_transmit_DMA = heavy duty display data transfer via DMA,
 * */

void sendCommand(uint8_t commandByte, const uint8_t *dataBytes,
                                  uint8_t numDataBytes) {
	/*8 bit method, not working in 16 bit mode*/
	spi3_set8();
	cs_enable();
	tft_dc_low();
	spi3_transmit(&commandByte,1); // Send the command byte
	tft_dc_high();
	if (numDataBytes >0 && dataBytes!=NULL){
		spi3_transmit(dataBytes,numDataBytes);

	}
	cs_disable();
	spi3_set16();
}

void sendCommand16(uint16_t commandByte, const uint16_t *dataBytes,
                                  uint16_t numDataBytes) {
	/* 16 bit method, goto for small commands that don't need a lot of data*/
	cs_enable();
	tft_dc_low();
	spi3_transmit16(&commandByte,1); // Send the command byte
	tft_dc_high();
	if (numDataBytes >0 && dataBytes!=NULL){
		spi3_transmit16(dataBytes,numDataBytes);
	}
	cs_disable();

}


void four_inch_init(void){
		uint8_t init_delay = 1;
		spi_gpio_init();
		systick_msec_sleep(init_delay);
		displayReset();
		systick_msec_sleep(init_delay);
		spi3_config();
		systick_msec_sleep(init_delay);
		spi_dma1_init(windowBuffer);
		systick_msec_sleep(init_delay);
		displayInit(st7796s_init);
		systick_msec_sleep(init_delay);
		spi3_set16();
		A0_init();
		A0_on();
	}

void displayInit(const uint8_t *addr) {
/*Interpreter for Display init Array*/
  uint8_t numCommands, cmd, numArgs;
  uint16_t ms;

  numCommands = *addr++; // Number of commands to follow
  while (numCommands--) {              // For each command...
    cmd = *addr++;       // Read command
    numArgs = *addr++;   // Number of args to follow
    ms = numArgs & ST_CMD_DELAY;       // If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;          // Mask out delay bit
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;

    if (ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if (ms == 255)
        ms = 500; // If 255, delay for 500 ms
      systick_msec_delay(ms);
    }
  }
}

void fullScreenColor(uint16_t color){
	setSingleColorStatus(1);
			initAdressWindow(0,0,DISPLAY_X_MAX,DISPLAY_Y_MAX);
			sendCommand16((uint16_t)ST77XX_RAMWR, NULL, 0);
			tft_dc_high();
			//set first windowBuffer to color
			windowBuffer[0] =  color;
			for (uint32_t line=0; line<DISPLAY_Y_MAX; line++){
				spi3_transmit_DMA(DISPLAY_X_MAX);
				}
			tft_dc_low();
}

void fillRectangle(uint16_t *buffer,int16_t x,int16_t y, uint8_t a, uint8_t b){
	//fills a rectangular screen area with the contents of *buffer

	setSingleColorStatus(0); //using buffer increment

	//crop logic
	int8_t cropOffset= 0;
	//check for completely out of bounds
	if((x+a<0) || (x >= DISPLAY_X_MAX ))return;
	if((y+b<0) || (y >= DISPLAY_Y_MAX ))return;

	//check for partially out of bounds

	if(x<0){
		cropOffset=x;
		a=a+x;
		x=0;
	}

	if((x+a)>DISPLAY_X_MAX){
		cropOffset=x+a-DISPLAY_X_MAX;
		a=DISPLAY_X_MAX - x;
	}

	//calculate total size to transmit
	uint16_t size = a*b;
	initAdressWindow(x, y, a, b);

	//fill windowBuffer
	if(cropOffset==0){
		//no cropping just write the buffer
	for (uint16_t i=0; i < size ;i++){
			windowBuffer[i]=buffer[i];
		}}
	else {
		return; //not handled properly at the moment
		}
	//sending Data via DMA
	sendCommand16((uint16_t)ST77XX_RAMWR, NULL, 0);
			tft_dc_high();
			spi3_transmit_DMA(size);
			tft_dc_low();
}

void fillRectangle_oneColor(uint16_t *buffer,int16_t x,int16_t y, uint8_t a, uint8_t b){
	//fills a rectangular area of the screen with just one color defined by buffer[0]
	//this way there is no need for the MPU to fill the whole buffer with a*b times the same element

	setSingleColorStatus(1); //no DMA increment in buffer, one byte is send

	//check for completely out of bounds
	if((x+a<0) || (x >= DISPLAY_X_MAX ))return;
	if((y+b<0) || (y >= DISPLAY_Y_MAX ))return;

	//calculate total size to transmit
	uint16_t size = a*b;
	initAdressWindow(x, y, a, b);

	//fill Buffer
	windowBuffer[0]=buffer[0];  //use only fist element because DMA increment is off

	//sending Data via DMA

	sendCommand16((uint16_t)ST77XX_RAMWR, NULL, 0);
			tft_dc_high();
			spi3_transmit_DMA(size);
			tft_dc_low();
}

void fillSquare_scaleup(uint16_t (*buffer)[8], uint16_t x, uint16_t y, uint16_t a){
	//fill a square of size 2a x 2a with the contents of *buffer,
	// use each pixel in buffer to fill 4 pixel in windowBuffer
	setSingleColorStatus(0); //single color mode =off, DMA increment buffer
	const uint8_t scale = 2;
	uint16_t size = a*a;
	for (uint8_t i=0; i < a;i++){
		for(uint8_t j=0;j<a;j++){
				uint16_t dst = i*2*2*a + j*2;
				windowBuffer[dst]=buffer[i][j];
				windowBuffer[dst + 1]=buffer[i][j];
				windowBuffer[dst + 2 *a]=buffer[i][j];
				windowBuffer[dst + 2*a +1 ]=buffer[i][j];
			}
	}
	fillRectangle(windowBuffer, x, y, 2*a,2*a);
}

void initAdressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	//define maximums for x,y
	uint16_t xmax = x + width - 1;
	uint16_t ymax = y + height - 1;

	//split into MSB/LSB
	uint8_t x_lo =(uint8_t)(x & 0xFF);
	uint8_t x_hi =(uint8_t) (x >> 8);
	uint8_t xmax_lo = (uint8_t)(xmax & 0xFF);
	uint8_t xmax_hi = (uint8_t)(xmax >> 8);

	uint8_t y_lo =(uint8_t) (y & 0xFF);
	uint8_t y_hi =(uint8_t) (y >> 8);
	uint8_t ymax_lo = (uint8_t)(ymax & 0xFF);
	uint8_t ymax_hi = (uint8_t)(ymax >> 8);

	//prepare and set caset/raset for display
	uint8_t caset_data[4] =  {x_hi ,x_lo,xmax_hi,xmax_lo};
	uint8_t raset_data[4] =  {y_hi ,y_lo,ymax_hi,ymax_lo};
	sendCommand((uint8_t)ST77XX_CASET,caset_data, 4);
	sendCommand((uint8_t)ST77XX_RASET, raset_data, 4);
	sendCommand((uint8_t)ST77XX_RAMWR, NULL, 0);
}

void testScreen_16(void){

	sendCommand16((uint16_t)ST77XX_RAMWR, NULL, 0);

			tft_dc_high();

			for	(uint16_t j=0;j < DISPLAY_Y_MAX;j++){
				uint8_t line= floor(j/40);
				for (uint32_t i=0; i<DISPLAY_X_MAX; i++){
					switch(line){
					case 0:
						lineBuffer[i] = COLOR16_WHITE;
						break;
					case 1:
						lineBuffer[i] = COLOR16_GREEN;
						break;
					case 2:
						lineBuffer[i] = COLOR16_BLACK;
						break;
					case 3:
						lineBuffer[i] = COLOR16_BLUE;
						break;
					case 4:
						lineBuffer[i] = COLOR16_LIGHTBLUE;
						break;
					case 5:
						lineBuffer[i] = COLOR16_RED;
						break;
				}
			}
				spi3_transmit16(lineBuffer,DISPLAY_X_MAX);
			}
			tft_dc_low();
}
void displayReset(void){
	//PC01
	//reset pin aus und wieder an
	GPIOC->ODR &=~ (1U<<1);
	systick_msec_sleep(20); //50ms war stabil, 20ms geraten, aber funktioniert
	GPIOC->ODR |= (1U<<1);
}
void cs_enable(void)/*Pull low to enable*/
{
	/*----not used at the moment---*/
	//GPIOA->ODR &=~(1U<<11);

}

void cs_disable(void) /*Pull high to disable*/
{
	/*----not used at the moment---*/
	//GPIOA->ODR |=(1U<<11);
}

void tft_dc_low(void){
	//PC0 for data/control selection
	GPIOC->ODR &=~(1U<<0);
}
void tft_dc_high(void){
	//PC0 for data/control selection
	GPIOC->ODR |=(1U<<0);
}

void setSingleColorStatus(bool singleColor){
	if(singleColor == singleColorStatus){
		//do nothing if already in right state
		return;
	}
	else{
		//set DMA mode
		spi3_dma_setSingleColorStatus(singleColor);
		//update buffer
		singleColorStatus = singleColor;
	}
}
