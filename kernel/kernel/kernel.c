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
#include <kernel/tty.h>
#include <kernel/bitmap.h>

extern void switch_to_user_mode(void(*func)());

int FINISHED_INIT = 0;

void kmain(uint32_t ebx) {	
	struct multiboot_header *mbt = (struct multiboot_header*) ebx;
	multiboot_init(mbt);
	gdt_init();
	isr_install();
	irq_install();
	pmm_init(mmap.total_memory);
	paging_init();
	heap_init();
	init_stdio(kmalloc(STDIO_SIZE, 0));
	vesa_graphics_init(mbt);
	tty_init();

	FINISHED_INIT = 1;

//	print_mmap();
	print_modules();
	kprint("CasteOSv2 kernel initialized.\n");
	kprint("Attempting to switch to user mode shell.\n");
	//asm volatile("xchg %bx, %bx");
	//create_process(0, PAGE_SIZE );
	switch_to_user_mode(get_module("MYPROGRAM")->mod_start);

	for(;;) {}
}

