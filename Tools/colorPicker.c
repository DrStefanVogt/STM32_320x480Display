#include <stdio.h>
#include <stdint.h>

int new=1;
uint16_t testC=0xA8FF;
uint8_t out[2];

typedef struct {
    uint8_t hi;
    uint8_t low;
} Color16;

uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) |   // Rot   -> Bits 15..11
           ((g & 0xFC) << 3) |   // Grün -> Bits 10..5
           ( b >> 3);            // Blau -> Bits 4..0

}

void color(uint16_t cin , uint8_t cout[2]) {
	cout[0] = cin >> 8;
    cout[1] = cin & 0x00FF;
}
int main(void){
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 139;
    uint16_t colorInput = color565(r,g,b);
    uint8_t Split[2];
    color(colorInput,Split);
    printf("color input:\n r: %hhu, g: %hhu, b: %hhu\n",r,g,b);
    printf("color output:\n hi:%hhu\nlow:%hhu",Split[0],Split[1]);
    printf("\n%u",3U);
    return 1;
}


