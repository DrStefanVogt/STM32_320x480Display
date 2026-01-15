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
	sbc_lcd01_init();
	testScreen_16();
	systick_msec_delay(500);
	static const char oneWord[]="HALLO JESSICA";
	while(1){
		//fullScreenColor(COLOR16_WHITE);
		//systick_msec_delay(500);
		fullScreenColor(COLOR16_BLUE);
		//systick_msec_sleep(1000);
		rectangle(120,50,120,120,COLOR16_RED);
		//systick_msec_sleep(1000);
		rectangle(0,50,80,80,COLOR16_BLACK);
		rectangle(0,50,80,80,COLOR16_BLACK);
		systick_msec_sleep(100);
		fullScreenColor(COLOR16_WHITE);
		writeWord(oneWord,220,160,COLOR16_RED,COLOR16_WHITE);
		//systick_ms++;
		systick_msec_sleep(5000);

	}

}


