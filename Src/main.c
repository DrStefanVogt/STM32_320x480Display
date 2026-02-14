#include "stm32f4xx.h"
#include "systick.h"
#include "gpio.h"
#include "spi.h"
#include "sbc_lcd01.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "spi_dma.h"
#include "myGFX.h"
#include "uart_dma.h"
#include "uart.h"

extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;
extern uint8_t g_uart_idle;

extern char pc_uart_data_buffer[DEBUG_UART_DATA_BUFF_SIZE];
extern char uart_dma_transfer_buffer[UART_TRANSFER_BUFFER_SIZE];
char msg_buff[UART_TRANSFER_BUFFER_SIZE] ={'\0'};
char GPGSV[15]={'\0'};



int main(void){
	four_inch_init();
	testScreen_16();
	debugSineCosine();
	systick_msec_delay(1000);
	rectangle_empty(0,0,230,230,10,COLOR16_BLUE);
	graphicsInit(COLOR16_GREEN, COLOR16_BLACK, 6);
	digitLCDInit(25,40,40,50,19,5);
	dma2_init();
	uart1_rx_tx_init();
	dma2_stream2_uart_rx_config();
	int16_t pos=0;
	uart_init();
	uint16_t number = 0;
	dma2_stream7_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));
	while(1){
		digitLCDUpdate(number);
		systick_msec_sleep(10);
		number++;

		if(g_uart_idle || g_tx_cmplt){

			for(uint16_t i=0;i<UART_TRANSFER_BUFFER_SIZE;i++){
				msg_buff[i] = uart_dma_transfer_buffer[i];
				if (uart_dma_transfer_buffer[i]=='\0') break;
			}
			g_rx_cmplt = 0;
			g_uart_idle = 0;
			printf("%s\r\n",msg_buff);
		}
	/*	printf(".");
		fflush(stdout);*/
		}

}


