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

static const bool debug = 1;
static const bool showall = 1;

extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;
extern uint8_t g_uart_idle;


extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[UART_DATA_BUFF_SIZE] ={'\0'}; //this will be obsolete after nmea_buffer is in
//extern char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];

#define STACK_SIZE 0x400


extern uint32_t _estack;

void stack_fill(void)
{
	//stack filled with 0xA5... to find stack usage for debugging
    uint32_t *stack_start = (uint32_t*)((uint8_t*)&_estack - STACK_SIZE);
    uint32_t *p = stack_start;

    while (p < &_estack)
    {
        *p++ = 0xA5A5A5A5;
    }
}

size_t stack_usage(void)
{
	//find stack that has not been overwritten since stack_fill()
    uint32_t *stack_start = (uint32_t*)((uint8_t*)&_estack - STACK_SIZE);
    uint32_t *p = stack_start;

    while (*p == 0xA5A5A5A5)
    {
        p++;
    }

    return (uint8_t*)&_estack - (uint8_t*)p;
}

volatile size_t usage; //variable for debugging stack issues

int main(void){

	A1_on();

	stack_fill();
	debugFillUartBuffer();
	SCB->CPACR |= (0xF << 20);  // Enable CP10 + CP11 for float
	usage  = stack_usage();
	printf("stack_usage: %i",usage);
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
	systick_msec_delay(500);
	uint16_t number = 0;
	fullScreenColor(COLOR16_WHITE);
	digitLCDInit(25,40,40,50,19,5);
	textInit(0,COLOR16_BLUE,COLOR16_WHITE);
	setGPGSV(1);
	uint8_t counter = 0;
	while (getTime()== 0 && counter < 10){
		init_nmea_buffer(uart_data_buffer);
		systick_msec_sleep(100);
		 if (debug) printf("waiting for GNSS...\r\n");
		systick_msec_sleep(100);
		counter++;
	}//wait for GNRMSentence to arrive
		dropAnchor((uint16_t)getTime(), getLattitude(),getLongitude());
	//debugGrid();
	uint16_t centerX=160;
	uint16_t centerY=250;
	uint8_t scale = 4; //in power of two 1=>2 2=>4 3=>8 4=>16
	uint8_t circle_m = 20;
	graphicsInit(COLOR16_RED, COLOR16_BLACK, 3);
	drawCircle(centerX,centerY,circle_m*100>>scale);

	while(1){
		digitLCDUpdate(number);

		usage  = stack_usage();
		number++;
		if (number%2500 == 0) nextColor();
		if(g_uart_idle){  //wait for end of NMEA Sentence transmisson, complete loop must be shorter than 1000ms
			g_uart_idle = 0;
			init_nmea_buffer(uart_data_buffer);
			if (debug && showall) printf("%s",uart_data_buffer);
			writeWord(getPositionSentence(),300,450);
			writeWord(getGSGSVSentence(0),300,440);
			drawUint16((uint16_t)usage,300,430,4);
			if (getAntennaStatus()) writeWord(" ANTENNA CONNECTED",300,420);
			else writeWord("INTERNAL ANTENNA",300,420);

			 if (debug){
				 printf("antenna: %i\r\n",getAntennaStatus());
				 printf("stack_usage: %i\r\n",usage);
				 printf("$GNRMC,%s\r\n",getGNRMCSentence());
				 printf("--->%i: %i,%i\r\n",(uint16_t)(getTime()),getLattitude(),getLongitude());
				 printf("Delta latt/lon(min/100000): %i,%i\r\n", getDeltaLatt(),getDeltaLon());
				 //for(uint8_t i=0;i<=NMEA_GPGSV_NUM;i++)printf("GPGSV %i: %s\r\n",i, getGSGSVSentence(i));
				 printf("r (m): %f,lat(cm): %i, lon(cm): %i\r\n",getDeltaMeter(),getDeltaLattCm(),getDeltaLonCm());
			 }
			drawSquare(centerX-(getDeltaLattCm()>>scale),centerY-(getDeltaLonCm()>>scale),3);

		}

	}

}
