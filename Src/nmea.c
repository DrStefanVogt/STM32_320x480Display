/*
 * nmea.c
 *
 *  Created on: Feb 17, 2026
 *      Author: stevo
 */
#include "nmea.h"

//char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];

typedef struct {
	char GPS_POS[NMEA_SENTENCE_LENGTH];
	char GNSS_POS[NMEA_SENTENCE_LENGTH];
} nmea_buffer;

static nmea_buffer n;

void init_nmea_buffer(char* uart_data){
	//	init
	n.GNSS_POS[0]= '\0';
	n.GPS_POS[0]= '\0';
	//going through the uart_data which is fed NMEA sentences by DMA, find valid sentences at collect them in struct nmea_buffer
	uint16_t uart_i = 0;
	while(uart_i < UART_DATA_BUFF_SIZE){
		bool sent_start = 0; //no active sentence detected
		uint8_t pos = 0;  //'cursor' position
		/*SEE IF SENTENCE HAS STARTED*/
		if(uart_data[uart_i] == '\0') break; //finish loop when end of uart_buffer is reached, TODO: check if NMEA may contain '\0'
		if(uart_data[uart_i] == '$'){
			pos++; //move cursor
			sent_start = 1; //start command given
		}
		else{
			uart_i++;
			continue;} //try next char
		if(uart_data[uart_i+pos] == 'G') pos++;
		else break; //second char should be G in all cases, but if it's not disregard this and move on to next '$'


		/*DETECT SENTENCE TYPE with next 4 chars*/
		uint32_t nmea_this = CMD4(uart_data[uart_i+pos],uart_data[uart_i+pos+1],uart_data[uart_i+pos+2],uart_data[uart_i+pos+3]);
		pos += 4; //skip 4 elements with cursor. cursor should be on first real data byte now

		char* writeTo_ptr = NULL; //pointer to right storage position
		switch(nmea_this){
			case CMD4('N','G','L','L'):
//				GNGLL = GNSS position
				writeTo_ptr = n.GPS_POS;
				break;
			case CMD4('P','G','L','L'):
//				GPS only
				writeTo_ptr = n.GNSS_POS;
				break;
			case CMD4('P','G','S','V'):
				writeTo_ptr = NULL; //Not yet used
				break;
			case CMD4('P','G','S','A'):
				writeTo_ptr = NULL; //Not yet used
				break;
		}
		//now I know sentence type, write all data until end of line '\r\n' to right position. End string with '\0'

		uint16_t remaining_data = UART_DATA_BUFF_SIZE - uart_i - pos; //this should be the number of elments between absolute cursor position and end of uart_buffer

		//loop that finally writes the data into the buffer
		for (uint8_t i=0;i<remaining_data;i++){ //for loop to avoid buffer overrun
			pos++;
			//check for end of statement
			if (uart_data[uart_i+pos]=='\r' || uart_data[uart_i+pos]=='\n'){
				writeTo_ptr[i] = '\0';
				uart_i +=pos;
				break;
			}
			else {
			writeTo_ptr[i]=uart_data[uart_i+pos];
			}

		}
	}
}
const char* getPositionSentence(void){
	if (n.GNSS_POS[0] == '\0') return n.GPS_POS;
	else return n.GNSS_POS;
}

