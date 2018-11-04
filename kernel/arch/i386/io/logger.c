#include <kernel/logger.h>

void klog(char* str) {
	int i;
	for(i = 0; str[i] != '\0'; i++) { klog_byte(str[i]); }
}

void klog_byte(char c) {
	port_byte_out(0x3F8, c);
}