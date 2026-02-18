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
#include "nmea.h"

extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;
extern uint8_t g_uart_idle;


extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[UART_DATA_BUFF_SIZE] ={'\0'}; //this will be obsolete after nmea_buffer is in
extern char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];



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
	fullScreenColor(COLOR16_WHITE);
	digitLCDInit(25,40,40,50,19,5);
	textInit(0,COLOR16_BLUE,COLOR16_WHITE);
	init_nmea_buffer();
//	debugGrid();
	while(1){
		digitLCDUpdate(number);
		systick_msec_sleep(10);
		number++;

		if(g_uart_idle){
			uint16_t j=0,k = 0;
			for(uint16_t i=0;i<UART_DATA_BUFF_SIZE;i++){
//				msg_buff[i] = uart_data_buffer[i];
				nmea_buffer[j][k] = uart_data_buffer[i];
				k++;
				if (uart_data_buffer[i]=='\n'){
					nmea_buffer[j][k]= '\0';
					j++;
					k = 0;
				}
				if (uart_data_buffer[i]=='\0') break;
			}
			g_rx_cmplt = 0;
			g_uart_idle = 0;

		identNMEASentence();
		writeWord(nmea_buffer[GPGLL],300,450);
		printf("*******************************\r\n");
		printf("%s",nmea_buffer[GPGLL]);
		printf("*******************************\r\n");
		for (uint8_t i=0;i<NMEA_BURST_NO ;i++){
			if (GPGSV[i]==0) break;
			writeWord(nmea_buffer[GPGSV[i]],300,430-i*10);
			printf("%s",nmea_buffer[GPGSV[i]]);

		}

		}
		for (uint8_t i=0;i<NMEA_BURST_NO;i++){
//			printf("%s",nmea_buffer[i]);
		}
	/*	printf(".");
		fflush(stdout);*/
		}

}


