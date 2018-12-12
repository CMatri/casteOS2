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
#include <kernel/multiboot.h>
#include <kernel/syscall.h>
#include <kernel/initrd.h>

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
	init_stdio();
	vesa_graphics_init(mbt);
	tty_init();
	fs_root = initrd_init(get_module("initrd")->mod_start);

	FINISHED_INIT = 1;

	print_modules();
	print_initrd();
	kprint("CasteOSv2 kernel initialized.\n");
	asm volatile("xchg %bx, %bx");

	//kprint("Attempting to switch to user mode shell.\n");
	//module_t* m = get_module("init_user");
	//create_process(m->mod_start, m->mod_end - m->mod_start);
	//shell();	
	tasking_init();
	for(;;) {}
}

