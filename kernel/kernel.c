#include "kernel.h"
#include "../cpu/pmm.h"
#include "../cpu/isr.h"
#include "../cpu/gdt.h"
#include "../cpu/heap.h"
#include "../cpu/ports.h"
#include "../cpu/paging.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include "../drivers/logger.h"

void kmain(uint32_t ebx) {	
	struct multiboot_header *mbt = (struct multiboot_header*) ebx;
	load_mmap(mbt);
	print_mmap(mmap);
	
	gdt_init();
	isr_install();
	irq_install();
	pmm_init(mmap.total_memory); // mmap.total_memory is total size in bytes but I'll stick with 64mb for now.
	paging_init();
	heap_init();
	
	kprint("Testing heap.\n");
	asm volatile("bpointdebug: ");
	uint8_t* ptr = (uint8_t*) kmalloc(PAGE_SIZE);
	uint8_t* ptr2 = (uint8_t*) kmalloc(1);
	khex(ptr);
	kprint("\n");
	khex(ptr2);
	kprint("\n");
	
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
