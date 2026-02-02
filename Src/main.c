#include "stm32f4xx.h"
#include "systick.h"
#include "gpio.h"
#include "spi.h"
#include "sbc_lcd01.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "spi_dma.h"
#include "myGFX.h"





int main(void){
	//sbc_lcd01_init();
	four_inch_init();
	debugSineCosine();
	A0_init();
	A0_on();
	systick_msec_delay(1500);
	//fullScreenColor(COLOR16_WHITE);
	rectangle_empty(0,0,230,230,10,COLOR16_BLUE);
	graphicsInit(COLOR16_GREEN, COLOR16_BLACK, 6);
	digitLCDInit(25,40,40,50,19,5);
	uint16_t number = 0;
	int16_t pos=0;

	while(1){
		digitLCDUpdate(number);
		systick_msec_sleep(50);
		pos =(pos+1)%450;
		number++;
		}

}


