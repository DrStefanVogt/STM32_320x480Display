/*
 * sbc_lcd01.c
 *
 *  Created on: Jan 3, 2026
 *      Author: stevo
 */
#include "spi.h"
#include "sbc_lcd01.h"
#include "exti.h"
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
         //    10 ms delay






void sendCommand(uint8_t commandByte, const uint8_t *dataBytes,
                                  uint8_t numDataBytes) {
	/*8 bit method, not working in 16 bit mode*/
	spi1_set8();
	cs_enable();
	tft_dc_low();
	spi1_transmit(&commandByte,1); // Send the command byte
	tft_dc_high();
	if (numDataBytes >0 && dataBytes!=NULL){
		spi1_transmit(dataBytes,numDataBytes);
	}
	cs_disable();
	spi1_set16();

}

void sendCommand16(uint16_t commandByte, const uint16_t *dataBytes,
                                  uint16_t numDataBytes) {
	cs_enable();
	tft_dc_low();
	spi1_transmit16(&commandByte,1); // Send the command byte
	tft_dc_high();
	if (numDataBytes >0 && dataBytes!=NULL){
		spi1_transmit16(dataBytes,numDataBytes);
	}
	cs_disable();

}

/*from adafruit_ST77cc.cpp*/
void sbc_lcd01_init(){
		uint8_t init_delay = 5;
		displayReset();
		systick_msec_sleep(init_delay);
		spi_gpio_init();
		systick_msec_sleep(init_delay);
		spi1_config();
		systick_msec_sleep(init_delay);
		spi_dma_init(windowBuffer);
		systick_msec_sleep(init_delay);
		displayInit(generic_st7789);
		systick_msec_sleep(init_delay);
		spi1_set16();
		systick_msec_sleep(init_delay);
		exti_init();
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
    //spi1_transmit(cmd,1);
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
			initAdressWindow(0,0,DISPLAY_X_MAX,DISPLAY_Y_MAX);
			sendCommand16((uint16_t)ST77XX_RAMWR, NULL, 0);
			tft_dc_high();
			for(uint32_t pixel=0;pixel<DISPLAY_X_MAX;pixel+=1) {
			//fill windowBuffer with color values
			windowBuffer[pixel] =  color;
			}
			for (uint32_t line=0; line<DISPLAY_Y_MAX; line++){
				spi1_transmit_DMA(DISPLAY_X_MAX);
				}
			tft_dc_low();
}

void fillRectangle(uint16_t *buffer,int16_t x,int16_t y, uint8_t a, uint8_t b){
	//crop logic
	if(x<0) x=0;
	if(y<0) y=0;

	//calculate total size to transmit
	uint16_t size = a*b;
	initAdressWindow(x, y, a, b);

	//fill windowBuffer
	for (uint16_t i=0; i < size ;i++){
		windowBuffer[i]=buffer[i];
	}
	//sending Data via DMA
	sendCommand16((uint16_t)ST77XX_RAMWR, NULL, 0);
			tft_dc_high();
			spi1_transmit_DMA(size);
			tft_dc_low();
}

void fillSquare_scaleup(uint16_t *buffer, uint16_t x, uint16_t y, uint16_t a){
	uint16_t size = a*a;
	const uint8_t scale = 2;
	uint16_t k=0;
	for (uint16_t i=0; i < size;i++){
			windowBuffer[scale*i+k]=buffer[i];
			windowBuffer[scale*i+k+1]=buffer[i];
			windowBuffer[scale*i+k+scale*a]=buffer[i];
			windowBuffer[scale*i+k+scale*a+1]=buffer[i];
			if (i!=0 && i%a==0) k+=scale *a;
			}
	fillRectangle(windowBuffer, x, y, 2*a,2*a);
}

void initAdressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height){

	uint8_t caset_data[4] =  {0 ,x,0,x+width-1};
	uint8_t raset_data[4] =  {0 ,y,0,y+height-1};
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
				spi1_transmit16(lineBuffer,DISPLAY_X_MAX);
			}
			tft_dc_low();
}
void displayReset(void){
	//reset pin aus und wieder an
	A0_init();
	A0_off();
	systick_msec_delay(50); //50ms ist geraten, aber funktioniert
	A0_on();
}
void cs_enable(void)/*Pull low to enable*/
{
	GPIOA->ODR &=~(1U<<11);

}

void cs_disable(void) /*Pull high to disable*/
{
	GPIOA->ODR |=(1U<<11);
}

void tft_dc_low(void){
	GPIOA->ODR &=~(1U<<9);
}
void tft_dc_high(void){
	GPIOA->ODR |=(1U<<9);
}
