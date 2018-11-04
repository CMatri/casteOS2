#ifndef LOGGER_H
#define LOGGER_H

#include <kernel/ports.h>

void klog(char* str);
void klog_byte(char c);

#endif