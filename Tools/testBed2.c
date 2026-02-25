#include <stdio.h>
#include <stdint.h>

#define NMEA_BURST_NO 25 //maximum number of NMEA sentences in one burst of sentences
#define NMEA_SENTENCE_LENGTH 90 //maximum number of chars in one NMEA sentence, https://en.wikipedia.org/wiki/NMEA_0183 says max 83 Chars
#define NMEA_ID_LENGTH 6 //NMEA idenification i.e. length($GPGSV)
#define NMEA_GPGSV_NUM 10 //maximum number of GPGSV sentences
#define NMEA_STATEMENTS_PER_SENTENCE 15 //maxium statements of NMEA sentence ($GXXXX,statement1,statement2,statement3...)
#define NMEA_CHARACTERS_PER_STATEMENT 11 //maximum characters per NMEA statement ($GXXXX,statement1,statement2,statement3...)

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


int main() {
  const char* testSentence = "131751.000,A,5429.4170,N,00943.9242,E,0.30,75.08,240226,,,A,V\0";
  char output[NMEA_STATEMENTS_PER_SENTENCE][NMEA_CHARACTERS_PER_STATEMENT];
  printf("testsentence: %s\n", testSentence);
  printf("output before: %s\n", output);
  splitNMEASentence(testSentence, output);
  for (int8_t i=0;i<NMEA_STATEMENTS_PER_SENTENCE;i++){
        printf("output after %i: %s\n",i, output[i]);
  }
  return 0;
}

