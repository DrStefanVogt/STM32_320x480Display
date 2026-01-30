#include "spi.h"


//PA5 -> CLK
//PA6 -> MISO
//PA7 -> MOSI

//PA9 -> Slave Select


void spi_gpio_init(void)
{
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA5,PA6,PA7 mode to alternate function*/

	/*PA5*/
	GPIOA->MODER &=~(1U<<10);
	GPIOA->MODER |=(1U<<11);

	/*PA6*/
	GPIOA->MODER &=~(1U<<12);
	GPIOA->MODER |=(1U<<13);

	/*PA7*/
	GPIOA->MODER &=~(1U<<14);
	GPIOA->MODER |=(1U<<15);

	/*PA8*/
	GPIOA->MODER |=(1U<<18);
	GPIOA->MODER &=~(1U<<19);

	/*PA10 as output*/
	GPIOA->MODER |=  (1U<<20);
	GPIOA->MODER &= ~(1U<<21);

	/*Set PA5,PA6,PA7 alternate function type to SPI1*/
	/*PA5*/
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


}

void spi1_config(void)
{
	/*Enable clock access to SPI1 module*/
	RCC->APB2ENR |= SPI1EN;

	//SPI1->CR1 &=~ (1<<6);

	/*Set clock to fPCLK/4*/
	//000=f/2 100=f/4 ...111=f/256
	if(0){SPI1->CR1 |=(1U<<3);}else{SPI1->CR1 &=~(1U<<3);}
	if(0){SPI1->CR1 |=(1U<<4);}else{SPI1->CR1 &=~(1U<<4);}
	if(0){SPI1->CR1 |=(1U<<5);}else{SPI1->CR1 &=~(1U<<5);}

	/*Set CPOL to 0 and CPHA to 0*/
	if(1){SPI1->CR1 |=(1U<<0);}else{SPI1->CR1 &=~(1U<<0);}
	if(1){SPI1->CR1 |=(1U<<1);}else{SPI1->CR1 &=~(1U<<1);}

	/*Enable full duplex*/
	SPI1->CR1 &=~(1U<<10);

	/*Set MSB first*/
	SPI1->CR1 &= ~(1U<<7);

	/*Set mode to MASTER*/
	SPI1->CR1 |= (1U<<2);
	/*Set mode to SLAVE*/
	//SPI1->CR1 &=~ (1U<<2);

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
void spi1_set16(void){
/*Set 16 bit data mode*/
SPI1->CR1 |=  (1U<<11);
}

void spi1_set8(void){
/*Set 16 bit data mode*/
SPI1->CR1 &=~  (1U<<11);
}
