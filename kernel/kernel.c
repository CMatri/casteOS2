#include <kernel/kernel.h>
#include <kernel/mem/pmm.h>
#include <kernel/cpu/isr.h>
#include <kernel/cpu/gdt.h>
#include <kernel/mem/heap.h>
#include <kernel/sys/ports.h>
#include <kernel/mem/paging.h>
#include <libc/string.h>
#include <kernel/io/vesa_graphics.h>
#include <kernel/io/screen.h>
#include <kernel/io/logger.h>

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
	graphics_init(mbt);

	kprint("\nConnor's kernel\n"
        "Type END to halt the CPU\n> ");
	klog("Kernel started.\n");
	for(;;){
		update_graphics();
	}
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
