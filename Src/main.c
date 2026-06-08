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

static const uint16_t anchorCircle_centerX=160;
static const uint16_t anchorCircle_centerY=250;
static const uint8_t scale = 4; //in power of two 1=>2 2=>4 3=>8 4=>16

extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;
extern uint8_t g_uart_idle;


extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[UART_DATA_BUFF_SIZE] ={'\0'}; //this will be obsolete after nmea_buffer is in
//extern char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];

#define STACK_SIZE 0x400

#define WAIT_TEXT_X 300
#define WAIT_TEXT_Y 460
#define WAIT_TEXT_Y_LINEFEED -10


extern uint32_t _estack;

enum stateMachine{
	WAIT_FOR_GPS,
	WAIT_FOR_ANCHOR_DROP,
	RECORD_POSITION
};

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

void drawAnchorCircle(uint8_t circle_m){
	graphicsSettings(COLOR16_RED, COLOR16_BLACK, 3);
	drawCircle(anchorCircle_centerX,anchorCircle_centerY,circle_m*100>>scale);
	return;
}

volatile size_t usage; //variable for debugging stack issues

enum stateMachine state = WAIT_FOR_GPS;

int main(void){

	//STM32 comunication initialize
	A1_on();
	SCB->CPACR |= (0xF << 20);  // Enable CP10 + CP11 for float
	dma2_init();
	uart1_rx_tx_init();
	dma2_stream2_uart_rx_config();
	uart_init();

	//stack usage monitoring, just for developement
	stack_fill();
	debugFillUartBuffer();
	usage  = stack_usage();
	if(debug) printf("stack_usage: %i",usage);

	//display init
	four_inch_init();
	if(debug)testScreen_16();
	debugSineCosine();
	systick_msec_delay(1000);
	fullScreenColor(COLOR16_WHITE);
	graphicsSettings(COLOR16_GREEN, COLOR16_BLACK, 6);
	digitLCDInit(25,40,40,50,19,5);
	textInit(0,COLOR16_BLUE,COLOR16_WHITE);
	drawAnchorCircle(25);
	debugGrid();

	uint16_t tickCounter = 0;
	setGPGSV(1);

	/* to be removed, keept as reference for the moment 20260604
	uint8_t counter = 0;
	while (getTime()== 0 && counter < 10){
		fill_nmea_buffer(uart_data_buffer);
		systick_msec_sleep(100);
		 if (debug) printf("waiting for GNSS...\r\n");
		systick_msec_sleep(100);
		counter++;
	}//wait for GNRMSentence to arrive
		dropAnchor((uint16_t)getTime(), getLattitude(),getLongitude());
	*/




	
	int32_t lattitude_now;
	int32_t longitude_now;
	uint16_t tick_at_last_nmea;
	while(1){
		digitLCDUpdate(tickCounter);
		tickCounter++;

		switch (state)
		{
		case WAIT_FOR_GPS:
			systick_msec_sleep(100);
			fill_nmea_buffer(uart_data_buffer);
			if(g_uart_idle && NMEAAlive()) {
				writeWord("GPS CONNECTED  ",WAIT_TEXT_X,WAIT_TEXT_Y+WAIT_TEXT_Y_LINEFEED);
				tick_at_last_nmea = tickCounter;
				g_uart_idle = 0;
			}
			else {
				if (tickCounter - tick_at_last_nmea > 10){
				writeWord("WAITING FOR GPS",WAIT_TEXT_X,WAIT_TEXT_Y+WAIT_TEXT_Y_LINEFEED);
				}
			}
			if(getLattitude()!=0){
				lattitude_now = getLattitude();
				longitude_now = getLongitude();
				printf("lattitude in WAIT: %li\r\n", lattitude_now);
				writeWord("LATTITUDE",WAIT_TEXT_X,440);
				drawInt32(lattitude_now,230,440,9);
				writeWord("LONGITUDE",WAIT_TEXT_X,430);
				drawInt32(longitude_now,230,430,9);
			}
			if (getTime()!= 0){
				 writeWord("GPS TIME: ",WAIT_TEXT_X,420);
				 drawUint16((uint16_t)getTime(),230,420,5);
			}
			printf("Number of Satelites:  %i\r\n",getSateliteInView());
			writeWord("SATTELITES IN VIEW:",WAIT_TEXT_X, WAIT_TEXT_Y);
			drawUint16((uint16_t)getSateliteInView(), WAIT_TEXT_X-(11*16),460,2);
			break;
		
		default:
			break;
		}
		if (!debug) continue;

		
		usage  = stack_usage();
	
		if (tickCounter%2500 == 0) nextColor();
		if(g_uart_idle){  //wait for end of NMEA Sentence transmisson, complete loop must be shorter than 1000ms
			g_uart_idle = 0;
			fill_nmea_buffer(uart_data_buffer);
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
				 printf("--->%i: %li,%li\r\n",(uint16_t)(getTime()),getLattitude(),getLongitude());
				 printf("Delta latt/lon(min/100000): %i,%i\r\n", getDeltaLatt(),getDeltaLon());
				 //for(uint8_t i=0;i<=NMEA_GPGSV_NUM;i++)printf("GPGSV %i: %s\r\n",i, getGSGSVSentence(i));
				 printf("r (m): %f,lat(cm): %i, lon(cm): %i\r\n",getDeltaMeter(),getDeltaLattCm(),getDeltaLonCm());
			 }
			drawSquare(anchorCircle_centerX-(getDeltaLattCm()>>scale),anchorCircle_centerY-(getDeltaLonCm()>>scale),3);

		}

	}

}
