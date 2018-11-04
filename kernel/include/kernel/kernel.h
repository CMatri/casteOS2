#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <kernel/multiboot.h>

#define BASE_VIRTUAL 0xC0000000

extern uint32_t KERNEL_VIRTUAL_BASE;
extern uint32_t kernel_virtual_end;
extern uint32_t kernel_physical_end;
extern uint32_t kernel_virtual_start;
extern uint32_t kernel_physical_start;
	
mmap_data_t mmap;

void user_input(char *input);

#endif