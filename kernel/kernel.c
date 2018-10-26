#include "kernel.h"
#include "../cpu/multiboot.h"
#include "../cpu/isr.h"
#include "../cpu/gdt.h"
#include "../cpu/ports.h"
#include "../cpu/paging.h"
#include "../drivers/screen.h"
#include "../drivers/logger.h"
#include "../libc/string.h"

void kmain() {
	gdt_init();
	isr_install();
	irq_install();
	
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