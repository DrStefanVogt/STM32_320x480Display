/*
 * gpio.c
 *
 *  Created on: Sep 12, 2025
 *      Author: stevo
 */
#include "gpio.h"

#define GPIOAEN			(1U<<0)
#define GPIOCEN			(1U<<2)

#define LED_BS5			(1U<<5)  /*Bit Set Pin 5*/
#define LED_BR5			(1U<<21) /*Bit Reset Pin 5*/

#define A0_BS0			(1U << 0) /*Bit Set Pin 0*/
#define A0_BR0			(1U << 16)/*Bit Reset Pin 0*/

#define BTN_PIN			(1U<<13)

void gpio_test(void)
{
	//very low level test to find right pins
    RCC->AHB1ENR |= (1U << 0);   // GPIOA clock enable

    // PA10 output
    GPIOA->MODER &= ~(3U << (10*2));
    GPIOA->MODER |=  (1U << (10*2));

    while (1)
    {
        GPIOA->ODR ^= (1U << 10);
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }
}

void led_init(void)
{
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA5 mode to output mode*/
	GPIOA->MODER |=(1U<<10);
	GPIOA->MODER &=~(1U<<11);
}

void A0_init(void){
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER |=(1U << 0);
	GPIOA->MODER &=~(1U<< 1);
}

void button_init(void)
{
	/*Enable clock access to PORTC*/
	RCC->AHB1ENR |=GPIOCEN;

	/*Set PC13 as an input pin*/
	GPIOC->MODER &=~(1U<<26);
	GPIOC->MODER &=~(1U<<27);

}


void led_on(void)
{
	/*Set PA5 high*/
	GPIOA->BSRR |=LED_BS5;
}

void led_off(void)
{
	/*Set PA5 high*/
	GPIOA->BSRR |=LED_BR5;

}


bool get_btn_state(void)
{

	/*Note : BTN is active low*/

	/*Check if button is pressed*/
	if(GPIOC->IDR & BTN_PIN)
	{
		return false;
	}
	else
	{
		return true;
	}

}

void A0_on(void){
	GPIOA->BSRR |= A0_BS0;
}

void A0_off(void){
	GPIOA->BSRR |= A0_BR0;
}
