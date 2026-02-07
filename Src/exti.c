/*
 * exti.c
 *
 *  Created on: Jan 9, 2026
 *      Author: stevo
 */


#include "exti.h"


#define GPIOCEN		    (1U<<2)
#define SYSCFGEN		(1U<<14)

void exti_init(void)
{
	/*Disable global interrupts*/
	__disable_irq();

	/*Enable clock access to SYSCFG*/
	RCC->APB2ENR |=SYSCFGEN;

	/*Select PORTC for EXTI13*/
	//Kollege GPT sagt das brauche ich nicht
	//SYSCFG->EXTICR[3] |=(1U<<5);

	DMA2_Stream3->CR |= (1U << 4); // TCIE
	DMA1_Stream5->CR |= (1U << 4); // TCIE
	/*Unmask EXTI13*/
	EXTI->IMR |=(1U<<13);

	/*Select falling edge trigger*/
	EXTI->FTSR |=(1U<<13);

	/*Enable DMA Stream in NVIC*/
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	/*Enable global interrupts*/
	__enable_irq();


}

void DMA2_Stream3_IRQHandler(void)
{
    // Clear IRQ
	DMA2->LIFCR |= (1U << 27);
}

void DMA1_Stream5_IRQHandler(void)
{
    // Clear IRQ
	DMA1->HIFCR |= (1U <<11);
}


