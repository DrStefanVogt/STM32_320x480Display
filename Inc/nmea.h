#ifndef NMEA_H
#define NMEA_H

#include <stdint.h>
#include <string.h>
#include "myGFX.h"

#define NMEA_BURST_NO 25 //maximum number of NMEA sentences in one burst of sentences
#define NMEA_SENTENCE_LENGTH 150 //maximum number of chars in one NMEA sentence
#define CMD4(a,b,c,d) ((uint32_t)(a)<<24 | (uint32_t)(b)<<16 | (uint32_t)(c)<<8 | (d))

extern char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];

extern uint8_t GPGLL;
extern uint8_t GPGSV[NMEA_BURST_NO];
extern uint8_t GPGSA[NMEA_BURST_NO];

void init_nmea_buffer(void);
void identNMEASencence(void);
#endif //NMEA_H
