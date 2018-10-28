#include "kernel.h"
#include "../cpu/isr.h"
#include "../cpu/gdt.h"
#include "../cpu/ports.h"
#include "../cpu/paging.h"
#include "../cpu/pmm.h"
#include "../drivers/screen.h"
#include "../drivers/logger.h"
#include "../libc/string.h"

void kmain(uint32_t ebx) {	
	struct multiboot_header *mbt = (struct multiboot_header*) ebx;
	load_mmap(mbt);
	print_mmap(mmap);
	
	gdt_init();
	isr_install();
	irq_install();
	pmm_init(mmap.total_memory_mb); // +1 because mmap.total_memory_mb is 1023 if 1G is dedicated. pmm_init requires a multiple of 4096.
	
    kprint("Connor's kernel\n"
        "Type END to halt the CPU\n> ");
	klog("Kernel started.\n");
	
	for(;;){}
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
	
    kprint("Shell: ");
    kprint(input);
    kprint("\n> ");
}
