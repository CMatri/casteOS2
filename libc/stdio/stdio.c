#include <stdio.h>
#include <stdlib.h>

void init_stdio() {
	int i;
	for(i = 0; i < STDIO_SIZE; i++) stdin[i] = 0;
}
