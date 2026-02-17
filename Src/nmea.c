/*
 * nmea.c
 *
 *  Created on: Feb 17, 2026
 *      Author: stevo
 */
#include "nmea.h"

#define CMD4(a,b,c,d) ((uint32_t)(a)<<24 | (uint32_t)(b)<<16 | (uint32_t)(c)<<8 | (d))


void identNMEASencence(void){
for (uint8_t i=0;i<NMEA_BURST_NO;i++){
	//read first 5 chars after $
	// GPGLL;
	// GPGSV[NMEA_BURST_NO-2];
	// GPGSA[NMEA_BURST_NO-2];

	if (nmea_buffer[i][0] != '$') break; //not a NMEA sentence
	uint32_t nmea_this = CMD4(nmea_buffer[i][2],nmea_buffer[i][3],nmea_buffer[i][4],nmea_buffer[i][5]);

}
}
