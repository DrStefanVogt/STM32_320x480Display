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
	SCB->CPACR |= (0xF << 20);  // Enable CP10 + CP11 for float
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
	setGPGSV(1);
	while (getTime()== 0){
		init_nmea_buffer(uart_data_buffer);
		systick_msec_sleep(100);
		printf("waiting for GNSS...\r\n");
		systick_msec_sleep(100);
	}//wait for GNRMSentence to arrive
	{
		float lat = getLattitude();
		float lon = getLongitude();
		dropAnchor((uint16_t)getTime(),lat,lon); //calling getL..() inside the dropAnchor(..) leads to crashes. buzzwords from chatGPT: ARM-ABI, S0/S1
	}
//	debugGrid();
	while(1){
		digitLCDUpdate(number);
		systick_msec_sleep(10);
		number++;
		if(g_uart_idle){  //wait for end of NMEA Sentence transmisson, complete loop must be shorter than 1000ms
			g_uart_idle = 0;
			init_nmea_buffer(uart_data_buffer);
			writeWord(getPositionSentence(),300,450);
//			printf("*******************************************\r\n%s\r\n*******************************************\r\n",getPositionSentence());
			printf("$GNRMC,%s\r\n",getGNRMCSentence());
//			printf("$GSGSV,%s\r\n", getGSGSVSentence(0));
//			printf("$GSGSV,%s\r\n", getGSGSVSentence(1));
//			printf("$GSGSV,%s\r\n", getGSGSVSentence(2));
			printf("%i: %f,%f\r\n",(uint16_t)(getTime()-163808.0f),getLattitude(),getLongitude());

		}

	}

}
