#include "spi.h"


//PA5 -> CLK --> switch to PC10
//PA6 -> MISO --> not needed if it were PC11
//PA7 -> MOSI --> switch to PC12


void spi_gpio_init(void)
{
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Enable GPIOC*/
	RCC->AHB1ENR |= GPIOCEN;

	/*Set PA5,PA6,PA7 mode to alternate function*/

	/*PA5 - legacy*/
	GPIOA->MODER &=~(1U<<10);
	GPIOA->MODER |=(1U<<11);

	/*PC10 -CLK*/
	GPIOC->MODER &=~(1U << 20);
	GPIOC->MODER |=(1U << 21);

	/*PA6-legacy*/
	GPIOA->MODER &=~(1U<<12);
	GPIOA->MODER |=(1U<<13);

	/*PA7 -legacy*/
	GPIOA->MODER &=~(1U<<14);
	GPIOA->MODER |=(1U<<15);

	/*PC12 - MOSI*/
	GPIOC->MODER &=~ (1U << 24);
	GPIOC->MODER |= (1U << 25);


	/*PA8 -legacy*/
	GPIOA->MODER |=(1U<<18);
	GPIOA->MODER &=~(1U<<19);

	/*PA10 as output -legacy*/
	GPIOA->MODER |=  (1U<<20);
	GPIOA->MODER &= ~(1U<<21);

	/*PC00 as output for tft_dc_low*/
	GPIOC->MODER |= (1U<<0);
	GPIOC->MODER &=~ (1U<<1);

	/*PC01 as output for display reset*/
	GPIOC->MODER |= (1U<<2);
	GPIOC->MODER &=~ (1U<<3);

	/*Set PA5,PA6,PA7 alternate function type to SPI1*/
	/*PA5 -legacy*/
	GPIOA->AFR[0] |=(1U<<20);
	GPIOA->AFR[0] &= ~(1U<<21);
	GPIOA->AFR[0] |=(1U<<22);
	GPIOA->AFR[0] &= ~(1U<<23);

	/*PA6*/
	GPIOA->AFR[0] |=(1U<<24);
	GPIOA->AFR[0] &= ~(1U<<25);
	GPIOA->AFR[0] |=(1U<<26);
	GPIOA->AFR[0] &= ~(1U<<27);

	/*PA7*/
	GPIOA->AFR[0] |=(1U<<28);
	GPIOA->AFR[0] &= ~(1U<<29);
	GPIOA->AFR[0] |=(1U<<30);
	GPIOA->AFR[0] &= ~(1U<<31);

	/*set PC10 and PC12 alternate function type to SPI3, AF06 from DS10314 REvB Tab9*/
	//PC10
	GPIOC->AFR[1] &=~ (1U<<8);
	GPIOC->AFR[1] |= (1U<<9);
	GPIOC->AFR[1] |= (1U<<10);
	GPIOC->AFR[1] &=~ (1U<<11);
	//PC12
	GPIOC->AFR[1] &=~ (1U<<16);
	GPIOC->AFR[1] |= (1U<<17);
	GPIOC->AFR[1] |= (1U<<18);
	GPIOC->AFR[1] &=~ (1U<<19);

}

void spi1_config(void)
{
	//all things doubled for SPI3 - only 95% sure that exchanging SPI1 with SPI3 works

	/*Enable clock access to SPI1 module*/
	RCC->APB2ENR |= SPI1EN;
	/*Enable clock access to SPI3 module*/
	RCC->APB1ENR |= SPI3EN;

	//SPI1->CR1 &=~ (1<<6);

	/*Set clock to fPCLK/4*/
	//000=f/2 100=f/4 ...111=f/256
	if(0){SPI1->CR1 |=(1U<<3);}else{SPI1->CR1 &=~(1U<<3);}
	if(0){SPI1->CR1 |=(1U<<4);}else{SPI1->CR1 &=~(1U<<4);}
	if(0){SPI1->CR1 |=(1U<<5);}else{SPI1->CR1 &=~(1U<<5);}

	/*Set CPOL to 0 and CPHA to 0*/
	if(0){SPI1->CR1 |=(1U<<0);}else{SPI1->CR1 &=~(1U<<0);}
	if(0){SPI1->CR1 |=(1U<<1);}else{SPI1->CR1 &=~(1U<<1);}

	/*Enable full duplex*/
	SPI1->CR1 &=~(1U<<10);

	/*Set MSB first*/
	SPI1->CR1 &= ~(1U<<7);

	/*Set mode to MASTER*/
	SPI1->CR1 |= (1U<<2);

	/*Set 8 bit data mode*/
	SPI1->CR1 &= ~(1U<<11);

	/*Set 16 bit data mode*/
	//SPI1->CR1 |=  (1U<<11);

	/*Select software slave management by
	 * setting SSM=1 and SSI=1*/
	SPI1->CR1 |= (1<<8);
	SPI1->CR1 |= (1<<9);


	/*Enable SPI module*/
	SPI1->CR1 |= (1<<6);

}

void spi3_config(void){
	//all things doubled for SPI3 - only 95% sure that exchanging SPI1 with SPI3 works

	/*Enable clock access to SPI1 module*/
	RCC->APB2ENR |= SPI1EN; //clock access still needed for background light to work, should be obsolete later
		/*Enable clock access to SPI3 module*/
		RCC->APB1ENR |= SPI3EN;

		/*Set clock to fPCLK/4*/
		//000=f/2 100=f/4 ...111=f/256
		if(0){SPI3->CR1 |=(1U<<3);}else{SPI3->CR1 &=~(1U<<3);}
		if(0){SPI3->CR1 |=(1U<<4);}else{SPI3->CR1 &=~(1U<<4);}
		if(0){SPI3->CR1 |=(1U<<5);}else{SPI3->CR1 &=~(1U<<5);}

		/*Set CPOL to 0 and CPHA to 0*/
		if(0){SPI3->CR1 |=(1U<<0);}else{SPI3->CR1 &=~(1U<<0);}
		if(0){SPI3->CR1 |=(1U<<1);}else{SPI3->CR1 &=~(1U<<1);}
		/*Enable full duplex*/
		SPI3->CR1 &=~(1U<<10); //
		/*Set MSB first*/
		SPI3->CR1 &= ~(1U<<7);
		/*Set mode to MASTER*/
		SPI3->CR1 |= (1U<<2);
		/*Set 8 bit data mode*/
		SPI3->CR1 &= ~(1U<<11);
		/*Select software slave management by
		 * setting SSM=1 and SSI=1*/
		SPI3->CR1 |= (1<<8);
		SPI3->CR1 |= (1<<9);
		/*Enable SPI module*/
		SPI3->CR1 |= (1<<6);

}

void spi1_transmit(uint8_t *data,uint32_t size)
{
	/*8 bit version for init only*/
	uint32_t i=0;
	uint8_t temp;

	while(i<size)
	{
		/*Wait until TXE is set*/
		while(!(SPI1->SR & (SR_TXE))){}

		/*Write the data to the data register*/
		SPI1->DR = data[i];
		i++;
	}
	/*Wait until TXE is set*/
	while(!(SPI1->SR & (SR_TXE))){}

	/*Wait for BUSY flag to reset*/
	while((SPI1->SR & (SR_BSY))){}

	/*Clear OVR flag*/
	temp = SPI1->DR;
	temp = SPI1->SR;
	/*use temp variable to clear warning list*/
	if(temp!=0) temp=0;
}

void spi3_transmit(uint8_t *data,uint32_t size)
{
	/*8 bit version for init only*/
	uint32_t i=0;
	uint8_t temp;

	while(i<size)
	{
		/*Wait until TXE is set*/
		while(!(SPI3->SR & (SR_TXE))){}

		/*Write the data to the data register*/
		SPI3->DR = data[i];
		i++;
	}
	/*Wait until TXE is set*/
	while(!(SPI3->SR & (SR_TXE))){}

	/*Wait for BUSY flag to reset*/
	while((SPI3->SR & (SR_BSY))){}

	/*Clear OVR flag*/
	temp = SPI3->DR;
	temp = SPI3->SR;
	/*use temp variable to clear warning list*/
	if(temp!=0) temp=0;
}

void spi1_transmit16(uint16_t *data,uint32_t size)
	{
		/*16 bit version*/
		uint32_t i=0;

		uint8_t temp;

		while(i<size)
		{
			/*Wait until TXE is set*/
			while(!(SPI1->SR & (SR_TXE))){}

			/*Write the data to the data register*/
			SPI1->DR = data[i];
			i++;
		}
		/*Wait until TXE is set*/
		while(!(SPI1->SR & (SR_TXE))){}

		/*Wait for BUSY flag to reset*/
		while((SPI1->SR & (SR_BSY))){}

		/*Clear OVR flag*/
		temp = SPI1->DR;
		temp = SPI1->SR;
		/*use temp variable to clear warning list*/
		if(temp!=0) temp=0;
	}

void spi3_transmit16(uint16_t *data,uint32_t size)
	{
		/*16 bit version*/
		uint32_t i=0;

		uint8_t temp;

		while(i<size)
		{
			/*Wait until TXE is set*/
			while(!(SPI3->SR & (SR_TXE))){}

			/*Write the data to the data register*/
			SPI3->DR = data[i];
			i++;
		}
		/*Wait until TXE is set*/
		while(!(SPI3->SR & (SR_TXE))){}

		/*Wait for BUSY flag to reset*/
		while((SPI3->SR & (SR_BSY))){}

		/*Clear OVR flag*/
		temp = SPI3->DR;
		temp = SPI3->SR;
		/*use temp variable to clear warning list*/
		if(temp!=0) temp=0;
	}


void spi1_receive(uint8_t *data,uint32_t size)
{
	while(size)
	{
		/*Send dummy data*/
		SPI1->DR =0;

		/*Wait for RXNE flag to be set*/
		while(!(SPI1->SR & (SR_RXNE))){}

		/*Read data from data register*/
		*data++ = (SPI1->DR);
		size--;
	}
}

void spi1_DMA_enable(void){
	SPI1->CR1 |= (1U << 6);
	SPI1->CR2 |= (1U<<1);
}

void spi3_DMA_enable(void){
	SPI3->CR1 |= (1U << 6); //SPI3 ist das wirklich richtig?!
	SPI3->CR2 |= (1U<<1);
}



void spi1_set16(void){
/*Set 16 bit data mode*/
SPI1->CR1 |=  (1U<<11);
}

void spi3_set16(void){
/*Set 16 bit data mode*/
SPI3->CR1 |=  (1U<<11);
}

void spi1_set8(void){
/*Set 16 bit data mode*/
SPI1->CR1 &=~  (1U<<11);
}

void spi3_set8(void){
/*Set 16 bit data mode*/
SPI3->CR1 &=~  (1U<<11);
}
