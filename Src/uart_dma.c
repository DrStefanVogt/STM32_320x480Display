#include "uart_dma.h"

#define UART1EN			(1U<<4)
#define UART2EN			(1U<<17)
#define GPIOAEN			(1U<<0)

#define CR1_TE			(1U<<3)
#define CR1_IDLEIE		(1U<<4)
#define CR1_RE			(1U<<2)
#define CR1_UE			(1U<<13)
#define SR_TXE			(1U<<7)

#define CR3_DMAT		(1U<<7)
#define CR3_DMAR		(1U<<6)
#define SR_TC			(1U<<6)
#define SR_IDLE			(1U<<4)
#define CR1_TCIE		(1U<<6)

#define UART_BAUDRATE	9600
#define CLK				16000000

#define DMA1EN			    (1U<<21)
#define DMA2EN				(1U<<22)
#define DMA_SCR_EN  		(1U<<0)
#define DMA_SCR_MINC		(1U<<10)
#define DMA_SCR_PINC		(1U<<9)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_TCIE		(1U<<4)
#define DMA_SCR_TEIE		(1U<<2)
#define DMA_SFCR_DMDIS		(1U<<2)

#define HIFCR_CDMEIF5		(1U<<8)
#define HIFCR_CTEIF5		(1U<<9)
#define HIFCR_CTCIF5		(1U<<11)

#define HIFCR_CDMEIF6		(1U<<18)
#define HIFCR_CTEIF6		(1U<<19)
#define HIFCR_CTCIF6		(1U<<21)

#define HIFSR_TCIF5		(1U<<11)
#define HIFSR_TCIF6		(1U<<21)

#define LIFCR_CTCIF2	(1U<<21)

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate);
static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);

char uart_data_buffer[UART_DATA_BUFF_SIZE];
char uart_dma_transfer_buffer[UART_TRANSFER_BUFFER_SIZE];

uint8_t g_rx_cmplt;
uint8_t g_tx_cmplt;

uint8_t g_uart_cmplt;
uint8_t g_uart_idle;

void uart1_rx_tx_init(void)
{
//	PA9...USART1_TX
//
	/*************Configure UART GPIO pin********************/
	/*1.Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*2.Set PA9 mode to alternate function mode*/
	GPIOA->MODER &= ~(1U<<18);
	GPIOA->MODER |=	 (1U<<19);

	/*3.Set PA10 mode to alternate function mode*/
	GPIOA->MODER &= ~(1U<<20);
	GPIOA->MODER |=	 (1U<<21);

	/*4.Set PA9 alternate function function type to AF7(UART1_TX)*/
	GPIOA->AFR[1] |= (1U<<4);
	GPIOA->AFR[1] |= (1U<<5);
	GPIOA->AFR[1] |= (1U<<6);
	GPIOA->AFR[1] &= ~(1U<<7);

	/*5.Set PA10 alternate function function type to AF7(UART1_TX)*/
	GPIOA->AFR[1] |= (1U<<8);
	GPIOA->AFR[1] |= (1U<<9);
	GPIOA->AFR[1] |= (1U<<10);
	GPIOA->AFR[1] &= ~(1U<<11);
	/*************Configure UART Module********************/

	/*6. Enable clock access to UART1*/
	RCC->APB2ENR |= UART1EN;

	/*7. Set baudrate*/
	uart_set_baudrate(CLK,UART_BAUDRATE);

//------------>
	/*8. Select to use DMA for TX and RX*/
	USART1->CR3 = CR3_DMAT |CR3_DMAR;

	/*9. Set transfer direction*/
	 USART1->CR1 = CR1_TE |CR1_RE;

	/*10.Clear TC flag*/
	 USART1->SR &=~SR_TC;
	 /*10a Clear IDLE flag*/
	 USART1->SR &=~SR_IDLE;

	 /*11a Enable IDLEI - idle interrupt*/
	 USART1->CR1 |= CR1_IDLEIE;

	/*11.Enable TCIE*/
	 USART1->CR1 |=CR1_TCIE;

	/*12. Enable uart module*/
	 USART1->CR1 |= CR1_UE;

	 /*13.Enable USART1 interrupt in the NVIC*/
	 NVIC_EnableIRQ(USART1_IRQn);

}


void dma2_init(void)
{
   /*Enable clock access to DMA*/
	RCC->AHB1ENR |=DMA2EN;

	/*Enable DMA2 Stream2 Interrupt in NVIC*/
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}


void dma2_stream2_uart_rx_config(void)
{
	/*Disable DMA stream*/
	DMA2_Stream2->CR &=~DMA_SCR_EN;

	/*Wait till DMA Stream is disabled*/
	while((DMA2_Stream2->CR & DMA_SCR_EN)){}

	/*Clear interrupt flags for stream 2*/
	DMA2->LIFCR |= (1U<<18);
	DMA2->LIFCR |= (1U<<19);
	DMA2->LIFCR |= (1U<<20);
	DMA2->LIFCR |= LIFCR_CTCIF2;

	/*Set periph address*/
	DMA2_Stream2->PAR = (uint32_t)(&(USART1->DR));

	/*Set mem address*/
	DMA2_Stream2->M0AR = (uint32_t)(&uart_data_buffer);

	/*Set number of transfer*/
	DMA2_Stream2->NDTR = (uint16_t)UART_DATA_BUFF_SIZE;

	/*Select Channel 4*/
	DMA2_Stream2->CR &= ~(1U<<25);
	DMA2_Stream2->CR &= ~(1U<<26);
	DMA2_Stream2->CR |= (1U<<27);

	/*Enable memory addr increment*/
	DMA2_Stream2->CR |=DMA_SCR_MINC;

	/*Enable transfer complete interrupt*/
	DMA2_Stream2->CR |= DMA_SCR_TCIE;

	/*Disble Circular mode*/
	DMA2_Stream2->CR &=~DMA_SCR_CIRC;

	/*Set transfer direction : Periph to Mem*/
	DMA2_Stream2->CR &=~(1U<<6);
	DMA2_Stream2->CR &=~(1U<<7);

	/*Enable DMA2 stream*/
	DMA2_Stream2->CR |= DMA_SCR_EN;

	/*Enable DMA2 Stream2 Interrupt in NVIC*/
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

void dma2_stream7_uart_tx_config(uint32_t msg_to_snd, uint32_t msg_len)
{
	/*Disable DMA stream*/
	DMA2_Stream7->CR &=~DMA_SCR_EN;

	/*Wait till  DMA Stream is disabled*/
	while((DMA2_Stream7->CR & DMA_SCR_EN)){}

	/*Clear interrupt flags for stream 7*/
	DMA2->HIFCR |= (1U<<24);
	DMA2->HIFCR |= (1U<<25);
	DMA2->HIFCR |= (1U<<26);
	DMA2->HIFCR |= (1U<<27);

	/*Set periph address*/
	DMA2_Stream7->PAR = (uint32_t)(&(USART1->DR));//19.6.2 RM0383 Rev4

	/*Set mem address*/
	DMA2_Stream7->M0AR = msg_to_snd;

	/*Set number of transfer*/
	DMA2_Stream7->NDTR = msg_len;

	/*Select Channel 4*/
	DMA2_Stream7->CR &= ~(1U<<25);
	DMA2_Stream7->CR &= ~(1U<<26);
	DMA2_Stream7->CR |= (1U<<27);

	/*Enable memory addr increment*/
	DMA2_Stream7->CR |=DMA_SCR_MINC;

	/*Set transfer direction :Mem to Periph*/
	DMA2_Stream7->CR |=(1U<<6);
	DMA2_Stream7->CR &=~(1U<<7);

	/*Set transfer complete interrupt*/
	DMA2_Stream7->CR |= DMA_SCR_TCIE;

	/*Enable DMA stream*/
	DMA2_Stream7->CR |= DMA_SCR_EN;

}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk +( baudrate/2U ))/baudrate);
}


static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	USART1->BRR  = compute_uart_bd(periph_clk,baudrate);
}


void DMA2_Stream7_IRQHandler(void)
{
	if((DMA2->HISR) & HIFSR_TCIF6)
	{
		//do_ssomething
		g_tx_cmplt = 1;
		/*Clear the flag*/
		DMA1->HIFCR |= HIFCR_CTCIF6;
	}
}


void DMA2_Stream2_IRQHandler(void)
{
//	receive data
	if((DMA2->LISR) & LIFCR_CTCIF2)
	{
		/*set complete flag*/
		g_rx_cmplt = 1;
		/*Clear the flag*/
		DMA2->LIFCR |= LIFCR_CTCIF2;
		printf("DMA2_Stream2 IRQ\r\n");
	}
}
void USART1_IRQHandler(void)
{
	if(USART1->SR & SR_TC){
		g_uart_cmplt  = 1;
		printf("TC interrupt\r\n");
		/*Clear TC interrupt flag*/
		USART1->SR &=~SR_TC;
	}
	if(USART1->SR & SR_IDLE){
		volatile uint32_t tmp;
		g_uart_idle  = 1;

		/*Clear TC interrupt flag*/
		USART1->SR &=~SR_TC;
		/*Clear IDLE interrupt flag*/
		tmp = USART1->SR; //flag is cleared by reading these two registers...read carfully
		tmp = USART1->DR;

		printf("idle interrupt\r\n");
		/*Disable DMA stream*/
		DMA2_Stream2->CR &=~DMA_SCR_EN;
		/*Wait till DMA Stream is disabled*/
		while((DMA2_Stream2->CR & DMA_SCR_EN)){}
		uint8_t data_rec = UART_DATA_BUFF_SIZE - DMA2_Stream2->NDTR;
		uint8_t end=1;
		for(uint8_t i =0;i<data_rec;i++){
			uart_dma_transfer_buffer[i] = uart_data_buffer[i];
			end++;
		}
		uart_dma_transfer_buffer[end]='\0';
		DMA2_Stream2->NDTR = UART_DATA_BUFF_SIZE;
		/*enable DMA Stream*/
		DMA2_Stream2->CR |= DMA_SCR_EN;

	}

	/*Clear TC interrupt flag*/
	USART1->SR &=~SR_TC;
	/*Clear IDLE interrupt flag*/
	USART1->SR &=~SR_IDLE;

}


