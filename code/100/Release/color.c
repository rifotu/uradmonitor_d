#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

uint16_t rgb24to16(uint8_t r, uint8_t g, uint8_t b) {
	return ((r>>3) << 11) | ((g>>2) << 5) | (b>>3);
}

int main(int argc, char *argv[]) {
	uint8_t r,g,b;
	char *hex = argv[1], tmp[3];
	strncpy(tmp, hex, 2);
	r = strtoul(tmp, 0, 16);
	strncpy(tmp, hex+2, 2);
	g = strtoul(tmp, 0, 16);
	strncpy(tmp, hex+4, 2);
	b = strtoul(tmp, 0, 16);
	
	//printf( "%d %d %d %s %x", r,g,b, hex, 1);
	printf("Components %s are %X, %X, %X\n16bit color is %4X\n", hex, r,g,b, rgb24to16(r,g,b));

	return 1;

}