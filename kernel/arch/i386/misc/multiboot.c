#include <kernel/multiboot.h>
#include <string.h>
#include <kernel/screen.h>
#include <kernel/logger.h>
#include <kernel/kernel.h>

char* MULTIBOOT_MMAP_TYPES[] = { "", "FREE_MEMORY", "RESERVED", "ACPI", "HIBERNATION", "BAD_CELL" };

void multiboot_init(struct multiboot_header *m) {
	mbt = m;
	load_modules();
	load_mmap();
}

module_t* get_module(char* string) {
	int i;
	for(i = 0; i < mbt->mods_count; i++) {
		if(memcmp(mods[i].string, string, strlen(string)) == 0) return &mods[i];
	}
	return 0;
}

void load_modules() {
	uint32_t i, addr;
	for(i = 0, addr = mbt->mods_addr + BASE_VIRTUAL; i < mbt->mods_count; i++, addr += sizeof(module_t)) {
		if(i >= MAX_MODULES) { kprint("Loading more than MAX_MODULES."); break; }
		
		module_t* mod = (module_t*) addr;
		if(mod) {
			memcpy(&mods[i], mod, sizeof(module_t));
			mods[i].mod_start += BASE_VIRTUAL;
			mods[i].mod_end   += BASE_VIRTUAL;
			mods[i].string 	  += BASE_VIRTUAL;
		}
	}
}

void print_modules() {
	int i;
	kprint("Loaded modules:\n");
	for(i = 0; i < mbt->mods_count; i++) {
		module_t m = mods[i];
		kprint((m.string));
		kprint(": 0x");
		khex(m.mod_start);
		kprint("-0x");
		khex(m.mod_end);
		kprint("\n");
	}
}

void load_mmap() {
	mmap.entries = (mboot_memmap_t*) (&kernel_virtual_end);
  	mmap.length = mbt->mmap_length / sizeof(mboot_memmap_t);
	memcpy((uint8_t*)mmap.entries, (uint8_t*)(mbt->mmap_addr + (uint32_t) &KERNEL_VIRTUAL_BASE), mbt->mmap_length);
	
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