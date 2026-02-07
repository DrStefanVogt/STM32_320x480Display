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
/*#include "uart_dma.h"
#include "uart.h"*/

/*
extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;

extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[150] ={'\0'};
*/


int main(void){
	//sbc_lcd01_init();
	four_inch_init();
	A0_init();
	A0_on();
	testScreen_16();
	debugSineCosine();
	systick_msec_delay(1500);
	rectangle_empty(0,0,230,230,10,COLOR16_BLUE);
	graphicsInit(COLOR16_GREEN, COLOR16_BLACK, 6);
	digitLCDInit(25,40,40,50,19,5);
	uint16_t number = 0;
	int16_t pos=0;
	uint32_t length=1;
	//uart_init();

	 //dma1_init();
	//uart2_rx_tx_init();
	//sprintf(msg_buff,"Initialization...cmplt\n\r");
	//dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));
	//while(!g_tx_cmplt){}

	while(1){
	//	sprintf(msg_buff, "Message received : %s \r\n",uart_data_buffer);
		digitLCDUpdate(number);
		systick_msec_sleep(50);
		pos =(pos+1)%450;
		number++;
		//g_tx_cmplt = 0;
		//uart_write(32);
		//sprintf(msg_buff,"number %d\n\r",number);
		//dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));
		//while(!g_tx_cmplt){}
		}

}


