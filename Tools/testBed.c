#include <stdio.h>
#include <stdint.h>

int new=1;
uint16_t testC=0xA8FF;
uint8_t out[2];


uint32_t A[2]={0x7E666666,0x3C3C667E};

uint32_t localBuffer[64];

int main() {
    for(int8_t j=1;j>=0;j--){
    for (int16_t i = 31 ;i>=0;i--){
    localBuffer[j*32+i]=A[j]>>i & 0x00000001;
        //printf("%d\t",j*32+i);
    }}
    printf("\n\n\n");
       for (int8_t i=63;i>=0;i--){
        if((i+1)%8==0) printf("\n");
        //printf("%d",i);
        printf("%d\t",i);
       
    } 
    printf("\n\n\n");
        for (uint8_t i = 0 ;i<64;i+=8){
            printf("\n");
    	    for(uint8_t j=0; j<8;j++){
                printf("%d\t",(i)+j);
            }
    } 

  return 0;
}

