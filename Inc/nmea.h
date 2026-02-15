#include <stdint.h>


#define NMEA_BURST_NO 15 //maximum number of NMEA sentences in one burst of sentences
#define NMEA_SENTENCE_LENGTH 150 //maximum number of chars in one NMEA sentence


char nmea_buffer[NMEA_BURST_NO][NMEA_SENTENCE_LENGTH];
