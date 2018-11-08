#include <string.h>
#include <stdio.h>
#include <kernel/kernel.h>
#include <kernel/pmm.h>
#include <kernel/isr.h>
#include <kernel/gdt.h>
#include <kernel/heap.h>
#include <kernel/ports.h>
#include <kernel/paging.h>
#include <kernel/vesa_graphics.h>
#include <kernel/screen.h>
#include <kernel/shell.h>
#include <kernel/logger.h>
#include <kernel/ps2mouse.h>
#include <kernel/bitmap.h>

void kmain(uint32_t ebx) {	
	struct multiboot_header *mbt = (struct multiboot_header*) ebx;
	load_mmap(mbt);
	print_mmap(mmap);
	
	gdt_init();
	isr_install();
	irq_install();
	pmm_init(mmap.total_memory);
	paging_init();
	heap_init();
	init_stdio(kmalloc_a(STDIO_SIZE));
	graphics_init(mbt);
	printf("CasteOSv2 kernel initialized. Beginning shell.\n");

	for(;;){
	//	shell();
		update_graphics();
	}
}

void user_input(char *input) {
	memcpy(stdin, input, strlen(input));
}
