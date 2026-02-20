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
//extern char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];



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
	uart_init();
	systick_msec_delay(10);
	uint16_t number = 0;
	fullScreenColor(COLOR16_WHITE);
	digitLCDInit(25,40,40,50,19,5);
	textInit(0,COLOR16_BLUE,COLOR16_WHITE);
	init_nmea_buffer(uart_data_buffer);
//	debugGrid();
	while(1){
		digitLCDUpdate(number);
		systick_msec_sleep(100);
		number++;
		if(g_uart_idle){
			g_uart_idle = 0;
			init_nmea_buffer(uart_data_buffer);
			writeWord(getPositionSentence(),300,450);
			//printf("%s\r\n", getPositionSentence());
		}

	}

}
