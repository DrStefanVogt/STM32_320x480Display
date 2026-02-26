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
	char GNRMC[NMEA_SENTENCE_LENGTH];
	char GNRMC_split[NMEA_STATEMENTS_PER_SENTENCE][NMEA_CHARACTERS_PER_STATEMENT];
	char GPGSV[NMEA_GPGSV_NUM][NMEA_SENTENCE_LENGTH];
	bool GPGSV_on;
	bool GNRMC_on;
} nmea_buffer;

typedef struct {
	float lattitude;
	float longitude;
	uint16_t time_seconds;
}anchor;

static nmea_buffer n;
static anchor a;

static bool debug =0;
static const int8_t hex_lut[256] = {
    ['0']=0,['1']=1,['2']=2,['3']=3,['4']=4,
    ['5']=5,['6']=6,['7']=7,['8']=8,['9']=9,
    ['A']=10,['B']=11,['C']=12,['D']=13,['E']=14,['F']=15,
    ['a']=10,['b']=11,['c']=12,['d']=13,['e']=14,['f']=15
};

static inline uint8_t min_u8(uint8_t a, uint8_t b) {
    return (a < b) ? a : b;
};

void init_nmea_buffer(char* uart_data){
	//this funciton sorts data of uart_data into the nmea_buffer struct.
	//this is not protected against change of uart_data i.e. by DMA
	//caller must make sure that uart_data stays constant for example by timing the sequence much shorter than 1s
	n.GNSS_POS[0]= '\0';
	n.GPS_POS[0]= '\0';
	n.GNRMC_on = 0;
	//going through the uart_data which is fed NMEA sentences by DMA, find valid sentences at collect them in struct nmea_buffer
	uint16_t uart_i = 0;
	uint8_t gpgsv_i = 0;
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

		char* writeTo_ptr = NULL; //pointer to right storage position, initialized to NULL
		if(!validate_nmea_checksum(&uart_data[uart_i])) continue;
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
//				Data about Satelites in view
				if(n.GPGSV_on)writeTo_ptr = n.GPGSV[gpgsv_i++];
				if(gpgsv_i > NMEA_GPGSV_NUM) gpgsv_i = 0;
				break;
			case CMD4('P','G','S','A'):
				writeTo_ptr = NULL; //Not yet used
				break;
			case CMD4('N','R','M','C'):
				writeTo_ptr  = n.GNRMC;
				n.GNRMC_on = 1;
				break;
		}
		//now I know sentence type, write all data until end of line '\r\n' to right position. End string with '\0'

		uint16_t remaining_data = UART_DATA_BUFF_SIZE - uart_i - pos; //this should be the number of elments between absolute cursor position and end of uart_buffer

		//loop that finally writes the data into the buffer
		for (uint8_t i=0;i<remaining_data;i++){ //for loop to avoid buffer overrun
			pos++;
			//check for end of statement, '*' means checksum is reached
			if (uart_data[uart_i+pos]=='*' ||uart_data[uart_i+pos]=='\r' || uart_data[uart_i+pos]=='\n'){
				writeTo_ptr[i] = '\0';
				uart_i +=pos;
				break;
			}
			else {
			writeTo_ptr[i]=uart_data[uart_i+pos];
			}

		}
	}
	if(n.GNRMC_on) splitNMEASentence(n.GNRMC,n.GNRMC_split); //since GNRMXC is the most important sentence it is always split and saved when it comes. Could by directly saved to bits for optimization, but not now.
}

void dropAnchor(uint16_t time_seconds,float lattitude, float longitude){
	a.time_seconds = time_seconds;
	a.lattitude = lattitude;
	a.longitude = longitude;
	return;
}


const char* getPositionSentence(void){
	//return GNSS position if available, else return GPS only position
	if (n.GNSS_POS[0] == '\0') return n.GPS_POS;
	else return n.GNSS_POS;
}

const char* getGNRMCSentence(void){
	if(!n.GNRMC_on) n.GNRMC[0]='\0';
	return n.GNRMC;
}

const char* getGSGSVSentence(uint8_t num){
	uint8_t i = min_u8(num,NMEA_GPGSV_NUM);
	return n.GPGSV[i];

}

float getLattitude(void){
	float lattitude = stringToFloat(n.GNRMC_split[2]);
	return lattitude;
}

float getLongitude(void){
	float lattitude = stringToFloat(n.GNRMC_split[4]);
	return lattitude;
}
float getTime(void){
	float time = stringToFloat(n.GNRMC_split[0]);
	return time;
}

bool validate_nmea_checksum(const char *sentence)
{
    uint8_t checksum = 0;
    uint8_t checksum_received=0;
    // Skip leading '$' if present
    if (*sentence == '$') {
        sentence++;
    }
    if (debug) printf("%s",sentence);
    // XOR until '*' or end of string
    while (*sentence && *sentence != '*') {
        checksum ^= (uint8_t)(*sentence);
        sentence++;
    }
    if(*sentence == '*') sentence++;
    checksum_received = read_from_hex(sentence);
    if (checksum_received == checksum) return 1;
    else{
    	printf("Checksum Error. %x, %x\r\n",checksum,checksum_received);
        printf("This sentence was: %s", sentence);
        return 0;
    }
}
void setGPGSV(bool on){
	n.GPGSV_on = on;
	//clear NMEA buffer
	if (!on){
		for(uint8_t i=0;i<NMEA_GPGSV_NUM;i++) n.GPGSV[i][0]= '\0';
		}
	}


uint8_t read_from_hex(const char *input){
	uint8_t output;
	output = (uint8_t) ((hex_lut[(uint8_t)input[0]] << 4) |  hex_lut[(uint8_t)input[1]]);
	return output;
}

void splitNMEASentence(const char *input, char output[NMEA_STATEMENTS_PER_SENTENCE][NMEA_CHARACTERS_PER_STATEMENT]){
	uint8_t k=0;
	uint8_t i=0;
	uint8_t j=0;
	while(input[i]!= '\0' && i< NMEA_SENTENCE_LENGTH){
	        output[k][j] = input[i];
	        if(input[i] == ','){
	            output[k][j] ='\0';
	            k++;
	            j=0;
	        }
	        if(input[i] != ','){
	            j++;
	        }
	        i++;
	    }
	    for (;k<NMEA_STATEMENTS_PER_SENTENCE;k++) output[k][0]='\0';
	}

float stringToFloat(const char *input){
	float output = 0.0f;
	int8_t sign;
	float scale=1.0f;

	if (*input == '-') sign=-1;
	else sign=1;

	while (*input && *input != '.'){
		output = output * 10.0f + (*input++ - '0');
	}
	if (*input == '.'){
		input++;
		while (*input){
			scale *= 0.1f;
			output += (*input++ -'0') * scale;
		}
	}

	return sign * output;
}
