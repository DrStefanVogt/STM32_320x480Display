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
	int32_t lattitude;
	int32_t longitude;
	uint16_t time_seconds;
}anchor;

static nmea_buffer n;
static anchor a;
static uint8_t latt_anchor; //lattitude for meter conversion of longitude

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
// cos_sqare_table: 0..255 corresponds to cos(x)²*255
static const uint8_t cos_sq_table[] = {255, 254, 254, 254, 253, 253, 252, 251, 250, 248, 247, 245, 243, 242, 240, 237, 235, 233, 230, 227, 225, 222, 219, 216, 212, 209, 205, 202, 198, 195, 191, 187, 183, 179, 175, 171, 166, 162, 158, 154, 149, 145, 140, 136, 131, 127, 123, 118, 114, 109, 105, 100, 96, 92, 88, 83, 79, 75, 71, 67, 63, 59, 56, 52, 49, 45, 42, 38, 35, 32, 29, 27, 24, 21, 19, 17, 14, 12, 11, 9, 7, 6, 4, 3, 2, 1, 1, 0, 0, 0};
static const uint8_t cos_table[] = {255, 254, 254, 254, 254, 254, 253, 253, 252, 251, 251, 250, 249, 248, 247, 246, 245, 243, 242, 241, 239, 238, 236, 234, 232, 231, 229, 227, 225, 223, 220, 218, 216, 213, 211, 208, 206, 203, 200, 198, 195, 192, 189, 186, 183, 180, 177, 173, 170, 167, 163, 160, 156, 153, 149, 146, 142, 138, 135, 131, 127, 123, 119, 115, 111, 107, 103, 99, 95, 91, 87, 83, 78, 74, 70, 65, 61, 57, 53, 48, 44, 39, 35, 31, 26, 22, 17, 13, 8, 4, };
static const float coordToMeters = 0.01852f;
static const float coordToMeters_sq = 0.0003429904f; //(1852 m/minute/100000)²


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
			if(!validate_nmea_checksum(&uart_data[uart_i]))
			{
				while(uart_data[uart_i++] !='$' || uart_data[uart_i] != '\0'){
				}
				continue;
			}
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
		volatile bool validated = validate_nmea_checksum(&uart_data[uart_i]);
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
				if(!n.GPGSV_on)break;
				else {
					if(gpgsv_i >= NMEA_GPGSV_NUM) gpgsv_i = 0;
					writeTo_ptr = n.GPGSV[gpgsv_i];
					gpgsv_i++;
					break;
				}
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

void dropAnchor(uint16_t time_seconds,int32_t lattitude,int32_t longitude){
	a.time_seconds = time_seconds;
	a.lattitude = lattitude;
	a.longitude = longitude;
	latt_anchor = (uint8_t)(lattitude/1000000);
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

int32_t getLattitude(void){
	int32_t lattitude = stringToU32e4(n.GNRMC_split[2]);
	return lattitude;
}

int32_t getLongitude(void){
	int32_t lattitude = stringToU32e4(n.GNRMC_split[4]);
	return lattitude;
}
float getTime(void){
	float time = stringToFloat(n.GNRMC_split[0]);
	return time;
}

int16_t getDeltaLatt(void){
	return getLattitude() - a.lattitude;
}

int16_t getDeltaLon(void){
	return getLongitude() - a.longitude;
}

int16_t getDeltaLattCm(void){
	float delta = (getLattitude() - a.lattitude)*coordToMeters*100;
	return (int16_t)delta;
}

int16_t getDeltaLonCm(void){
	float delta = (getLongitude() - a.longitude)*coordToMeters*100*cos_table[latt_anchor]/255;
	return (int16_t)delta;
}

float getDeltaMeter(void){
	float deltaMeter;
	int16_t lat = getDeltaLatt();
	int16_t lon = getDeltaLon();
	deltaMeter = sqrtf((lat*lat*coordToMeters_sq/255)+(lon*lon*coordToMeters_sq*cos_sq_table[latt_anchor]/255));
	printf("%i\r\n",latt_anchor);
	return deltaMeter;
}

bool validate_nmea_checksum(const char *sentence)
{
	//this function gives 1 if the received checksum after * matches expectations from received sentence
    uint8_t checksum = 0;
    uint8_t checksum_received=0;
    // Skip leading '$' if present, if no '$' the sentence is considered corrupt
    if (*sentence == '$') {
        sentence++;
    }
    else return 0;

    // XOR until '*' or end of string
    for(uint8_t i=0;i<NMEA_SENTENCE_LENGTH;i++){
    	if (*sentence == '*') break;
    	checksum ^=(uint8_t)(*sentence);
    	sentence++;
    	if (*sentence == '\r' || *sentence =='\n' || *sentence == '\0') return 0;
    }


    //if checksum mark '*' is present go on else invalid senentece
    if(*sentence == '*') sentence++;
    else return 0;

    if (isxdigit(sentence[0]) && isxdigit(sentence[1])) {
    	checksum_received = read_from_hex(sentence);
        }
    else return 0;

    if (checksum_received == checksum) return 1;
    else{
    	 if (debug) printf("Checksum Error. %x, %x\r\n",checksum,checksum_received);
    	 if (debug) printf("This sentence was: %s", sentence);
        return 0;
    }
    return 0; //something went very wrong if this executes
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

int32_t stringToU32e4(const char *input){
	int32_t output = 0;
	int8_t sign = 1;
	int8_t e4scale = 4;

	if (*input == '-') sign=-1;

	while (*input && *input != '.'){
		output = output * 10 + (*input++ - '0');
	}
	if (*input == '.'){
		input++;
		while (e4scale > 0){
			e4scale--;
            if (*input)	output = output * 10 + (*input++ - '0');
            else output *= 10;
		}
	}
	return sign * output;
}
