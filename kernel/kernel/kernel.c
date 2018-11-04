#include <string.h>
#include <kernel/kernel.h>
#include <kernel/pmm.h>
#include <kernel/isr.h>
#include <kernel/gdt.h>
#include <kernel/heap.h>
#include <kernel/ports.h>
#include <kernel/paging.h>
#include <kernel/vesa_graphics.h>
#include <kernel/screen.h>
#include <kernel/logger.h>

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
    if (memcmp(input, "END", sizeof(input)) == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }

    kprint("Shell: ");
    kprint(input);
    kprint("\n> ");
}
