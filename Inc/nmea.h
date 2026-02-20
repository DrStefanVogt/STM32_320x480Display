#ifndef NMEA_H
#define NMEA_H

#include <stdint.h>
#include <string.h>
#include "myGFX.h"
#include "uart_dma.h"

#define NMEA_BURST_NO 25 //maximum number of NMEA sentences in one burst of sentences
#define NMEA_SENTENCE_LENGTH 90 //maximum number of chars in one NMEA sentence, https://en.wikipedia.org/wiki/NMEA_0183 says max 83 Chars
#define NMEA_ID_LENGTH 6

#define CMD4(a,b,c,d) ((uint32_t)(a)<<24 | (uint32_t)(b)<<16 | (uint32_t)(c)<<8 | (uint32_t)(d))




extern uint8_t GPGLL;
extern uint8_t GPGSV[NMEA_BURST_NO];
extern uint8_t GPGSA[NMEA_BURST_NO];

void init_nmea_buffer(char* uart_data);
void identNMEASentence(uint32_t nmea_this);
const char* getPositionSentence(void);
bool validate_nmea_checksum(const char *sentence);
uint8_t read_from_hex(const char *input);
#endif //NMEA_H
