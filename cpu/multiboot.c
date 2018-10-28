#include "multiboot.h"
#include "../libc/mem.h"
#include "../drivers/screen.h"
#include "../kernel/kernel.h"

const char* MULTIBOOT_MMAP_TYPES[] = { "", "FREE_MEMORY", "RESERVED", "ACPI", "HIBERNATION", "BAD_CELL" };


void load_mmap(struct multiboot_header *mbt) {
	mmap.entries = (mboot_memmap_t*) (&kernel_virtual_end);
  	mmap.length = mbt->mmap_length / sizeof(mboot_memmap_t);
	memory_copy((char*)(mbt->mmap_addr + 0xC0000000), (char*)mmap.entries, mbt->mmap_length);
	
	int i;
	uint64_t size = 0;
	for(i = 0; i < mmap.length; i++) size += mmap.entries[i].length;
	size /= 1024 * 1024; // Convert to MB
	mmap.total_memory_mb = (uint32_t) size;
}

void print_mmap() {
	klog("======= MEMORY MAP =======\n");
	
	int i;
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
	
	kprint("Available Memory (MB): 0x");
	klog("Available Memory (MB): 0x");
	khex(mmap.total_memory_mb);
	klhex(mmap.total_memory_mb);
	klog("\n==========================\n");
	kprint("\n");
}