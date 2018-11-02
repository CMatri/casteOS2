#include "multiboot.h"
#include "../libc/mem.h"
#include "../drivers/screen.h"
#include "../drivers/logger.h"
#include "../kernel/kernel.h"

char* MULTIBOOT_MMAP_TYPES[] = { "", "FREE_MEMORY", "RESERVED", "ACPI", "HIBERNATION", "BAD_CELL" };

void load_mmap(struct multiboot_header *mbt) {
	mmap.entries = (mboot_memmap_t*) (&kernel_virtual_end);
  	mmap.length = mbt->mmap_length / sizeof(mboot_memmap_t);
	memory_copy((uint8_t*)(mbt->mmap_addr + (uint32_t) &KERNEL_VIRTUAL_BASE), (uint8_t*)mmap.entries, mbt->mmap_length);
	
	uint32_t i;
	uint64_t size = 0;
	for(i = 0; i < mmap.length; i++) size += mmap.entries[i].length;
	mmap.total_memory = (uint32_t) size;
}

void print_mmap() {
	klog("======= MEMORY MAP =======\n");
	kprint("MEM: ");
	klog("MEM: ");
	kdec(mmap.total_memory / 1024 / 1024);
	kldec(mmap.total_memory / 1024 / 1024);
	kprint(" MB (0x");
	klog(" MB (0x");
	khex(mmap.total_memory);
	klhex(mmap.total_memory);
	kprint(" bytes)\n");	
	klog(" bytes)");
	uint32_t i = 0;
	for(; i < mmap.length; i++) {
		mboot_memmap_t m = mmap.entries[i];
		klog("\n--------------------------\n");
		klog("Base addr: ");
		klhex(m.base_addr_low);
		klhex(m.base_addr_high);
		klog("\nLength: ");
		klhex((uint32_t) (m.length >> 32));
		klhex((uint32_t) m.length);
		klog("\nType: ");
		klog(MULTIBOOT_MMAP_TYPES[m.type]);
	}
	klog("\n==========================\n");
}