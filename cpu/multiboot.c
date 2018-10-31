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
	
	uint32_t i;
	for(i = 0; i < mmap.length; i++) {
		mboot_memmap_t m = mmap.entries[i];
		klog("Base addr: ");
		klhex(m.base_addr_low);
		klhex(m.base_addr_high);
		klog("\nLength: ");
		klhex((uint32_t) (m.length >> 32));
		klhex((uint32_t) m.length);
		klog("\nType: ");
		klog(MULTIBOOT_MMAP_TYPES[m.type]);
		klog("\n--------------------------\n");
	}
	
	kprint("Available Memory: 0x");
	klog("Available Memory: 0x");
	khex(mmap.total_memory);
	klhex(mmap.total_memory);
	klog("\n==========================\n");
	kprint("\n");
}