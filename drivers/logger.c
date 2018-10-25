#include "logger.h"

void klog(char* str) {
	int i;
	for(i = 0; str[i] != '\0'; i++) { port_byte_out(0x3F8, str[i]); }
}