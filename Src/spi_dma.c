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

//---> hier gehts weiter mit umstellung auf DMA2 und SPI3

void spi_dma2_init(uint16_t *buffer){

	/*Enable clock access to DMA2*/
	RCC->AHB1ENR |=DMA2EN;


	/*enable DMA for SPI1*/
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

void spi_dma1_init(uint16_t *buffer){
	//hier sind noch alle inits für 1 u 2 drin

	/*Enable clock access to DMA2*/
	RCC->AHB1ENR |=DMA2EN;

	/*Enable clock access to DMA1*/
	RCC->AHB1ENR |=DMA1EN;

	/*enable DMA2 for SPI1*/
	spi1_DMA_enable();

	/*enable DMA1 for SPI3*/
	spi3_DMA_enable();

	/*Disable DMA2 stream*/
	DMA2_Stream3->CR &=~DMA_SCR_EN; /*stream3 wegen tabelle 28 RM0383 171/842*/

	/*Disable_DMA1 stream*/
	DMA1_Stream5->CR &=~DMA_SCR_EN; /*Stream5wegen tabelle 28 RM0383 171/842*/

	/*wait till DMA is disabled*/
	while((DMA2_Stream3->CR & DMA_SCR_EN)){}

	/*wait till DMA is disabled*/
	while((DMA1_Stream5->CR & DMA_SCR_EN)){}

	/*Disable Circular mode*/
	DMA2_Stream3->CR &=~DMA_SCR_CIRC;
	//DMA2_Stream3->CR |= DMA_SCR_CIRC;

	/*Disable Circular mode*/
	DMA1_Stream5->CR &=~DMA_SCR_CIRC;
	//DMA2_Stream3->CR |= DMA_SCR_CIRC;

	/*Set MSIZE i.e Memory data size to half-word (= 16bit)*/
	DMA2_Stream3->CR |= (1U<<13);
	DMA2_Stream3->CR &= ~(1U<<14);

	/*Set MSIZE i.e Memory data size to half-word (= 16bit)*/
	DMA1_Stream5->CR |= (1U<<13);
	DMA1_Stream5->CR &= ~(1U<<14);

	/*Set PSIZE i.e Peripheral data size to half-word*/
	DMA2_Stream3->CR |= (1U<<11);
	DMA2_Stream3->CR &= ~(1U<<12);

	/*Set PSIZE i.e Peripheral data size to half-word*/
	DMA1_Stream5->CR |= (1U<<11);
	DMA1_Stream5->CR &= ~(1U<<12);

	/*set Data transfer direction*/
	DMA2_Stream3->CR |= DMA_CR_DIR1;
	DMA2_Stream3->CR &=~ DMA_CR_DIR2;

	/*set Data transfer direction*/
	DMA1_Stream5->CR |= DMA_CR_DIR1;
	DMA1_Stream5->CR &=~ DMA_CR_DIR2;


	/*Enable memory addr increment*/
	DMA2_Stream3->CR |=DMA_SCR_MINC;
	//DMA2_Stream3->CR &=~DMA_SCR_MINC;

	/*Enable memory addr increment*/
	DMA1_Stream5->CR |=DMA_SCR_MINC;
	//DMA1_Stream5->CR &=~DMA_SCR_MINC;

	/*Set periph address*/
	DMA2_Stream3->PAR = (uint32_t)(&(SPI1->DR)); // SPI data register

	/*Set periph address*/
	DMA1_Stream5->PAR = (uint32_t)(&(SPI3->DR)); // SPI data register

	/*Set mem address*/
	DMA2_Stream3->M0AR = (uint32_t)(buffer); //pointer to buffer, i.e. linebuffer

	/*Set mem address*/
	DMA1_Stream5->M0AR = (uint32_t)(buffer); //pointer to buffer, i.e. linebuffer

	/*chanel selection*/
	DMA2_Stream3->CR &=~ DMA_CR_CHSEL7; //all bits to 0
	DMA2_Stream3->CR |= DMA_CR_CHSEL3; // bit 25,26 to 1 for chanel3

	/*chanel selection*/
	DMA1_Stream5->CR &=~ DMA_CR_CHSEL7; //all bits to 0 for ch0

	/*clear all transfer complete */
	DMA2->LIFCR |= (1U<<24);
	DMA2->LIFCR |= (1U<<25);
	DMA2->LIFCR |= (1U<<26);
	DMA2->LIFCR |= (1U<<27);

	/*clear all transfer complete */
	DMA1->HIFCR |= (1U<<2);
	DMA1->HIFCR |= (1U<<3);
	DMA1->HIFCR |= (1U<<4);
	DMA1->HIFCR |= (1U<<5);

    /*Enable DMA stream*/
//	DMA2_Stream3->CR |= DMA_SCR_EN;

    /*Enable DMA stream*/
//	DMA1_Stream5->CR |= DMA_SCR_EN;

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
	    uint32_t dma1st4;
	   	uint16_t ndtr;
	    bool whilecond;
	     bool txd = SPI1->CR2 & (1U <<1);
	     bool spe = SPI1->CR1 & (1U <<6);
	     uint32_t chsel = DMA2_Stream3->CR & DMA_CR_CHSEL7;
	     DMA2->HIFCR |= DMA_HIFCR_CTCIF4;
	    while(DMA2_Stream3->CR & DMA_SCR_EN){
		      dma1st4= DMA1_Stream5->CR;
		      whilecond =DMA1_Stream5->CR & DMA_SCR_EN;
		      ndtr = DMA1_Stream5->NDTR;
	    __WFI(); //wait for interupt, if not right interrupt wait again
	    }
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
		DMA1->HIFCR |= (1U<<12);
	    /* Set number of data items */
	    DMA1_Stream5->NDTR = size;
	    /* Enable stream */
	    DMA1_Stream5->CR |= DMA_SCR_EN;
	    uint32_t dma1st4;
	    uint16_t ndtr;
	    bool whilecond;
	    bool txd = SPI3->CR2 & (1U <<1);
	    bool spe = SPI3->CR1 & (1U <<6);
	    uint32_t chsel = DMA1_Stream5->CR & DMA_CR_CHSEL7;
	    DMA1->HIFCR |= DMA_HIFCR_CTCIF4;
	    while(DMA1_Stream5->CR & DMA_SCR_EN){
	      dma1st4= DMA1_Stream5->CR;
	      whilecond =DMA1_Stream5->CR & DMA_SCR_EN;
	      ndtr = DMA1_Stream5->NDTR;
	    __WFI(); //wait for interupt, if not right interrupt wait again
	    }
	}

void spi1_dma_setSingleColorStatus(bool on){
	 /* Disable stream */
	DMA2_Stream3->CR &= ~DMA_SCR_EN;
	while (DMA2_Stream3->CR & DMA_SCR_EN){};
	/*Disable memory addr increment*/
	if(on)	DMA2_Stream3->CR &=~DMA_SCR_MINC;
	else DMA2_Stream3->CR |=DMA_SCR_MINC;
}

void spi3_dma_setSingleColorStatus(bool on){
	 /* Disable stream */
	DMA1_Stream5->CR &= ~DMA_SCR_EN;
	while (DMA1_Stream5->CR & DMA_SCR_EN){};
	/*Disable memory addr increment*/
	if(on)	DMA1_Stream5->CR &=~DMA_SCR_MINC;
	else DMA1_Stream5->CR |=DMA_SCR_MINC;
}
