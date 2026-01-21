/*
 * spi_dma.c
 *
 *  Created on: Jan 6, 2026
 *      Author: stevo
 *
 */
#include "spi_dma.h"
#include "sbc_lcd01.h"

#define DMA2EN				(1U<<22)
#define DMA_SCR_EN  		(1U<<0)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_MINC		(1U<<10)
#define DMA_CR_DIR1			(1U<<6)
#define DMA_CR_DIR2			(1U<<7)
#define DMA_CR_CHSEL7		(7U<<25)
#define DMA_CR_CHSEL3		(3U<<25)



void spi_dma_init(uint16_t *buffer){

	/*Enable clock access to DMA*/
	RCC->AHB1ENR |=DMA2EN;

	spi1_DMA_enable();

	/*Disable DMA stream*/
	DMA2_Stream3->CR &=~DMA_SCR_EN; /*stream3 wegen tabelle 28 RM0383 171/842*/

	/*wait till DMA is disabled*/
	while((DMA2_Stream3->CR & DMA_SCR_EN)){}

	/*Disable Circular mode*/
	DMA2_Stream3->CR &=~DMA_SCR_CIRC;
	//DMA2_Stream3->CR |= DMA_SCR_CIRC;

	/*Set MSIZE i.e Memory data size to half-word (= 16bit)*/

	DMA2_Stream3->CR |= (1U<<13);
	DMA2_Stream3->CR &= ~(1U<<14);

	/*Set PSIZE i.e Peripheral data size to half-word*/
	DMA2_Stream3->CR |= (1U<<11);
	DMA2_Stream3->CR &= ~(1U<<12);

	/*set Data transfer direction*/
	DMA2_Stream3->CR |= DMA_CR_DIR1;
	DMA2_Stream3->CR &=~ DMA_CR_DIR2;

	/*Enable memory addr increment*/
	DMA2_Stream3->CR |=DMA_SCR_MINC;
	//DMA2_Stream3->CR &=~DMA_SCR_MINC;

	/*Set periph address*/
	DMA2_Stream3->PAR = (uint32_t)(&(SPI1->DR)); // SPI data register

	/*Set mem address*/
	DMA2_Stream3->M0AR = (uint32_t)(buffer); //pointer to buffer, i.e. linebuffer


	/*chanel selection*/
	DMA2_Stream3->CR &=~ DMA_CR_CHSEL7; //all bits to 0
	DMA2_Stream3->CR |= DMA_CR_CHSEL3; // bit 25,26 to 1 for chanel3

	/*clear all transfer complete */
	DMA2->LIFCR |= (1U<<24);
	DMA2->LIFCR |= (1U<<25);
	DMA2->LIFCR |= (1U<<26);
	DMA2->LIFCR |= (1U<<27);

    /*Enable DMA stream*/
	DMA2_Stream3->CR |= DMA_SCR_EN;


}

void spi1_transmit_DMA(uint32_t size)
	{
		if(size==0) return;
		/* Disable stream */
	    DMA2_Stream3->CR &= ~DMA_SCR_EN;
	    while (DMA2_Stream3->CR & DMA_SCR_EN){};
	    /* Clear all relevant flags */
		DMA2->LIFCR |= (1U<<24);
		DMA2->LIFCR |= (1U<<25);
		DMA2->LIFCR |= (1U<<26);
		DMA2->LIFCR |= (1U<<27);
	    /* Set number of data items */
	    DMA2_Stream3->NDTR = size;
	    /* Enable stream */
	    DMA2_Stream3->CR |= DMA_SCR_EN;
	    //while(!(DMA2->LISR & (1U<<27))){}
	    while(DMA2_Stream3->CR & DMA_SCR_EN == 1){
	    __WFI(); //wait for interupt, if not right interrupt wait again
	    }
	}

void spi_dma_setSingleColorBuffer(bool on){
	 /* Disable stream */
	DMA2_Stream3->CR &= ~DMA_SCR_EN;
	while (DMA2_Stream3->CR & DMA_SCR_EN){};
	/*Disable memory addr increment*/
	if(on)	DMA2_Stream3->CR &=~DMA_SCR_MINC;
	else DMA2_Stream3->CR |=DMA_SCR_MINC;
}
