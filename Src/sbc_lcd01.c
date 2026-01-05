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

static uint16_t buf[BUFFER_BYTES];

Color16 white = {0xFF,0xFF};
Color16 green = {0x00,0x1F};
Color16 black = {0x00,0x00};
Color16 blue  = {17,0} ;
Color16 lightblue = {0xFF,0xAA};
Color16 red = {0,248};

Color16 colorScheme(uint8_t num){
	switch(num){
		case 0:
			return white;
		case 1:
			return blue;
		case 2:
			return red;

		default:
			return black;
	}
}

void sendCommand(uint8_t commandByte, const uint8_t *dataBytes,
                                  uint8_t numDataBytes) {
	cs_enable();
	tft_dc_low();
	spi1_transmit(&commandByte,1); // Send the command byte
	tft_dc_high();
	if (numDataBytes >0 && dataBytes!=NULL){
		spi1_transmit(dataBytes,numDataBytes);
	}
	cs_disable();

}

/*from adafruit_ST77cc.cpp*/
void displayInit(const uint8_t *addr) {

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

void cs_enable(void)
{
	GPIOA->ODR &=~(1U<<11);

}

/*Pull high to disable*/
void cs_disable(void)
{
	GPIOA->ODR |=(1U<<11);
}

void tft_dc_low(void){
	GPIOA->ODR &=~(1U<<9);//???
}
void tft_dc_high(void){
	GPIOA->ODR |=(1U<<9);//???
}

void sbc_lcd01_init(){
		//reset pin aus und wieder an
		A0_init();
		A0_off();
		systick_msec_delay(50); //50ms ist geraten, aber funktioniert
		A0_on();

		systick_msec_delay(5);
		spi_gpio_init();
		systick_msec_delay(5);
		spi1_config();
		systick_msec_delay(5);
		displayInit(generic_st7789);
	}

void fullScreenColor(uint8_t enumCol){
			sendCommand(ST77XX_RAMWR, NULL, 0);
			tft_dc_high();
			for(uint32_t i=0;i<BUFFER_PIXEL;i+=1) {
			switch(enumCol){
			case 0:
				buf[i] =  COLOR16_WHITE;
				//buf[i+1] =  white.hi;
				break;
			case 1:
				buf[i] =  COLOR16_BLACK;
				//buf[i+1] =  red.hi;
				break;
			case 2:
				buf[i] =  COLOR16_RED;
				//buf[i+1] =  blue.hi;
				break;
			default:
			//	uint8_t buf[2] =  {black.low,black.hi};
			}}
			for (uint32_t i=0; i<DISPLAY_CHUNKS; i++){
					//spi1_transmit(buf,BUFFER_BYTES);
				spi1_transmit_16(buf,BUFFER_PIXEL);
				}
			tft_dc_low();
}

void testScreen(void){
	sendCommand(ST77XX_CASET, (uint8_t[]){0,0,0,240}, 4);
	sendCommand(ST77XX_RASET, (uint8_t[]){0,0,1,63}, 4);
	sendCommand(ST77XX_RAMWR, NULL, 0);

			tft_dc_high();
			uint8_t pixel_color[2] = {white.low,white.hi};
			for (uint32_t i=0; i<240*320; i++){
				int j=floor(i/(40*240));
				switch(j){
				case 0:
					pixel_color[0] = white.low;
					pixel_color[1] = white.hi;
					break;
				case 1:
					pixel_color[0] = green.low;
					pixel_color[1] = green.hi;
					break;
				case 2:
					pixel_color[0] = black.low;
					pixel_color[1] = black.hi;;
					break;

				case 3:
					pixel_color[0] = blue.low;
					pixel_color[1] = blue.hi;
					break;
				case 4:
					pixel_color[0] = lightblue.low;
					pixel_color[1] = lightblue.hi;
					break;
				case 5:
					pixel_color[0] = red.low;
					pixel_color[1] = red.hi;
					break;
				}
				spi1_transmit(pixel_color,2);
			}

			tft_dc_low();
}
