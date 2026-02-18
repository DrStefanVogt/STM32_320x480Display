/*
 * nmea.c
 *
 *  Created on: Feb 17, 2026
 *      Author: stevo
 */
#include "nmea.h"

char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];

uint8_t GPGLL=0;
uint8_t GPGSV[NMEA_BURST_NO];
uint8_t GPGSA[NMEA_BURST_NO];

void init_nmea_buffer(void){
	uint8_t sv_counter=0;
	uint8_t sa_counter=0;
	memset(GPGSV, 0, sizeof(GPGSV));
	memset(GPGSA, 0, sizeof(GPGSA));
	memset(nmea_buffer,'\0', NMEA_BURST_NO*NMEA_SENTENCE_LENGTH);
}

void identNMEASentence(void){
uint8_t sv_counter=0;
uint8_t sa_counter=0;
memset(GPGSV, 0, sizeof(GPGSV));
memset(GPGSA, 0, sizeof(GPGSA));

for (uint8_t i=0;i<NMEA_BURST_NO;i++){

	if (nmea_buffer[i][0] != '$' || nmea_buffer[i][1] != 'G' ) continue; //not a NMEA sentence or not a GPS statement

	uint32_t nmea_this = CMD4(nmea_buffer[i][2],nmea_buffer[i][3],nmea_buffer[i][4],nmea_buffer[i][5]);

	switch(nmea_this){
		case CMD4('N','G','L','L'):
//				GNGLL = GNSS position
				GPGLL = i;
				break;
		case CMD4('P','G','L','L'):
//				GPS only
				GPGLL = i;
				break;
		case CMD4('P','G','S','V'):
				GPGSV[sv_counter] = i ;
				sv_counter = sv_counter+1;
				break;
		case CMD4('P','G','S','A'):
				GPGSA[sa_counter] = i ;
				sa_counter++;
				break;
	}

}
}
