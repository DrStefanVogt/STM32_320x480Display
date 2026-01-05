#include "stm32f4xx.h"
#include "systick.h"
#include "gpio.h"
#include "spi.h"
#include "sbc_lcd01.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

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
	testScreen();
	uint8_t n;
	while(1){
		fullScreenColor(n);
		n++;
		n=n%3;
		systick_msec_delay(1000);

	}

}
