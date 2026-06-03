/*
 * systick.c
 *
 *  Created on: Sep 10, 2025
 *      Author: stevo
 */
#include "systick.h"

#define CTRL_ENABLE		(1U<<0)
#define CTRL_CLCKSRC	(1U<<2)
#define CTRL_COUNTFLAG	(1U<<16)
#define CTRL_TICKINT (1U << 1)

/* by default f MCU is 16MHz*/
#define ONE_MSEC_LOAD	16000

volatile uint32_t systick_ms = 0;

void systick_msec_delay(uint32_t delay)
{
//Polling delay, uses a lot of power. Better use systick_msec_sleep() instead
//TODO: remove this function
    /*Load the timer with number of clock cycles per millisecond*/
	SysTick->LOAD =  ONE_MSEC_LOAD - 1;

    /*Clear systick current value register*/
	SysTick->VAL = 0;

    /*Select internal clock source*/
	SysTick->CTRL = CTRL_CLCKSRC;

	/*Enable systick*/
	SysTick->CTRL |=CTRL_ENABLE;

 	for(int i = 0; i < delay; i++)
	{
		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
	}

	/*Disable systick*/
	SysTick->CTRL = 0;

}

void systick_init(void){
		/*enable iterrupts*/
		__enable_irq();

		/*Load the timer with number of clock cycles per millisecond*/
			SysTick->LOAD =  (ONE_MSEC_LOAD - 1);

			/*Clear systick current value register*/
			SysTick->VAL = 0;

		    /*Select internal clock source*/
			SysTick->CTRL = CTRL_CLCKSRC;

			/*activate Iterrupts*/
			SysTick->CTRL |= CTRL_TICKINT;

			/*Enable systick*/
			SysTick->CTRL |=CTRL_ENABLE;

}

void systick_msec_sleep(uint32_t time){
	/*Load the timer with number of clock cycles per millisecond*/
			SysTick->LOAD =  (ONE_MSEC_LOAD - 1);

			/*Clear systick current value register*/
			//SysTick->VAL = 0;

		    /*Select internal clock source*/
			SysTick->CTRL = CTRL_CLCKSRC;

			/*activate Iterrupts*/
			SysTick->CTRL |= CTRL_TICKINT;

			/*Enable systick*/
			SysTick->CTRL |=CTRL_ENABLE;

		/*reset systick_ms*/
		systick_ms = 0;
		/*check every 1ms if time is up*/
		while(systick_ms < time){
			__WFI();
		}

}

void SysTick_Handler(void){
	systick_ms++;
}
