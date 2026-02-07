/*
 * spi_dma.c
 *
 *  Created on: Jan 6, 2026
 *      Author: stevo
 *
 */
#include "spi_dma.h"
#include "sbc_lcd01.h"

#define DMA1EN				(1U<<21)
#define DMA2EN				(1U<<22)
#define DMA_SCR_EN  		(1U<<0)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_MINC		(1U<<10)
#define DMA_CR_DIR1			(1U<<6)
#define DMA_CR_DIR2			(1U<<7)
#define DMA_CR_CHSEL7		(7U<<25)
#define DMA_CR_CHSEL3		(3U<<25)
#define DMA_CR_CHSEL4		(1U<<27)


void spi_dma1_init(uint16_t *buffer){
	//hier sind noch alle inits für 1 u 2 drin

	/*Enable clock access to DMA1*/
	RCC->AHB1ENR |=DMA1EN;

	/*enable DMA1 for SPI3*/
	spi3_DMA_enable();


	/*Disable_DMA1 stream*/
	DMA1_Stream5->CR &=~DMA_SCR_EN; /*Stream5wegen tabelle 28 RM0383 171/842*/

	/*wait till DMA is disabled*/
	while((DMA1_Stream5->CR & DMA_SCR_EN)){}

	/*Disable Circular mode*/
	DMA1_Stream5->CR &=~DMA_SCR_CIRC;
	//DMA2_Stream3->CR |= DMA_SCR_CIRC;

	/*Set MSIZE i.e Memory data size to half-word (= 16bit)*/
	DMA1_Stream5->CR |= (1U<<13);
	DMA1_Stream5->CR &= ~(1U<<14);

	/*Set PSIZE i.e Peripheral data size to half-word*/
	DMA1_Stream5->CR |= (1U<<11);
	DMA1_Stream5->CR &= ~(1U<<12);

	/*set Data transfer direction*/
	DMA1_Stream5->CR |= DMA_CR_DIR1;
	DMA1_Stream5->CR &=~ DMA_CR_DIR2;

	/*Enable memory addr increment*/
	DMA1_Stream5->CR |=DMA_SCR_MINC;
	//DMA1_Stream5->CR &=~DMA_SCR_MINC;

	/*Set periph address*/
	DMA1_Stream5->PAR = (uint32_t)(&(SPI3->DR)); // SPI data register

	/*Set mem address*/
	DMA1_Stream5->M0AR = (uint32_t)(buffer); //pointer to buffer, i.e. linebuffer

	/*chanel selection*/
	DMA1_Stream5->CR &=~ DMA_CR_CHSEL7; //all bits to 0 for ch0
	/*clear all transfer complete */
	DMA1->HIFCR |= (1U<<8);
	DMA1->HIFCR |= (1U<<9);
	DMA1->HIFCR |= (1U<<10);
	DMA1->HIFCR |= (1U<<11);

	/*Disable global interrupts*/
	__disable_irq();


	DMA1_Stream5->CR |= (1U << 4); // TCIE

	/*Enable DMA Stream in NVIC*/
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	/*Enable global interrupts*/
	__enable_irq();



}


void spi3_transmit_DMA(uint32_t size)
	{
		if(size==0) return;
		/* Disable stream */
	    DMA1_Stream5->CR &= ~DMA_SCR_EN;
	    while (DMA1_Stream5->CR & DMA_SCR_EN){};
	    /* Clear all relevant flags */
		DMA1->HIFCR |= (1U<<8);
		DMA1->HIFCR |= (1U<<9);
		DMA1->HIFCR |= (1U<<10);
		DMA1->HIFCR |= (1U<<11);
	    /* Set number of data items */
	    DMA1_Stream5->NDTR = size;
	    /* Enable stream */
	    DMA1_Stream5->CR |= DMA_SCR_EN;
	    while(DMA1_Stream5->CR & DMA_SCR_EN){
	    __WFI(); //wait for interupt, if not right interrupt wait again
	    }
	}


void spi3_dma_setSingleColorStatus(bool on){
	 /* Disable stream */
	DMA1_Stream5->CR &= ~DMA_SCR_EN;
	while (DMA1_Stream5->CR & DMA_SCR_EN){};
	/*Disable memory addr increment*/
	if(on)	DMA1_Stream5->CR &=~DMA_SCR_MINC;
	else DMA1_Stream5->CR |=DMA_SCR_MINC;
}

void DMA1_Stream5_IRQHandler(void)
{
    // Clear IRQ
	DMA1->HIFCR |= (1U <<11);
}


