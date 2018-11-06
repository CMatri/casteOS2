#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void init_stdio(uint8_t* inbuffer) {
	stdin = (uint8_t*) inbuffer;

	for(int i = 0; i < STDIO_SIZE; i++) inbuffer[i] = 0;
}
