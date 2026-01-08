#include "stm32f4xx.h"
#include "systick.h"
#include "gpio.h"
#include "spi.h"
#include "sbc_lcd01.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "spi_dma.h"

#define GPIOAEN		(1U<<0)

/*#define PIN5		(1U<<5)

#define LED_PIN		PIN5
*/



bool btn_state;

uint8_t data = 1;

int main(void){
	data = (uint8_t)200;
	if(0){
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*PA6 output mode 10*/
	GPIOA->MODER |=(1U<<12);
	GPIOA->MODER &=~(1U<<13);
	}
	sbc_lcd01_init();

	testScreen_16();
	uint8_t n =0;
	uint16_t start=0;
	uint16_t stop=0;
	systick_msec_delay(100);
	start += DMA2_Stream3->NDTR; //das ist an dieser Stelle 0 denn NDTR ist schon runtergezählt, HIER GEHT ES WEITER!
	spi_dma_test();
	for (volatile int i = 0; i < 10; i++);
	stop += DMA2_Stream3->NDTR;
	while(1){
		//fullScreenColor(n);
		//n++;
		//n=n%3;
		//systick_msec_delay(1000);
		//if (start==stop)fullScreenColor(2);
		//else fullScreenColor(1);
		systick_msec_delay(100);
		fullScreenColor(2);
		systick_msec_delay(100);
				fullScreenColor(0);
	}

}
