
// continue from https://github.com/dipolukarov/osdev/blob/master/paging.c
#include "isr.h"
#include "pmm.h"
#include "paging.h"
#include "isr.h"
#include "../libc/mem.h"
#include "../libc/function.h"
#include "../kernel/kernel.h"
#include "../drivers/screen.h"

static page_directory_t* page_directory = 0;
static uint32_t page_dir_loc = 0;
uint8_t* tmp_heap;

void* kmalloc_a(uint32_t size) {
	void* ret = tmp_heap;
	if(!IS_ALIGN(ret)) ret = (void*) PAGE_ALIGN(ret);
	tmp_heap += size;
	return ret;
}

void map_virtual_address(page_directory_t* dir, uint32_t vaddr, uint32_t paddr) {
	uint32_t pd_idx = (uint32_t) vaddr >> 22;
	uint32_t pt_idx = (uint32_t) (vaddr >> 12) & 0x03FF;
	
	if(!dir) {
		kpanic("Attempt to map page to NULL directory.\n");
		return;
	}
	
	if(!dir->ref_tables[pd_idx]) {		
		page_table_t* t = kmalloc_a(sizeof(page_table_t));
		memory_set((uint8_t*) t, 0, sizeof(page_table_t));
		uint32_t t_paddr = (uint32_t) t - BASE_VIRTUAL;
		
		dir->tables[pd_idx].present = 1;
		dir->tables[pd_idx].user = 1;
		dir->tables[pd_idx].rw = 1;
		dir->tables[pd_idx].page_size = 0;
		dir->tables[pd_idx].frame = t_paddr >> 12;
		dir->ref_tables[pd_idx] = t;
	}
	
	page_table_t* table = dir->ref_tables[pd_idx];
	if(!table->pages[pt_idx].present) {
		uint32_t addr = paddr ? paddr : allocate_block();
		table->pages[pt_idx].present = 1;
		table->pages[pt_idx].rw = 1;
		table->pages[pt_idx].user = 1;
		table->pages[pt_idx].frame = addr >> 12;		
	}
}

void load_page_dir(uint32_t dir) {
    asm volatile("mov %0, %%cr3" :: "r"(dir));
    
	uint32_t cr0, cr4;
	asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 = cr4 & 0xffffffef;
    asm volatile("mov %0, %%cr4" :: "r"(cr4));

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 = cr0 | 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
	
	kprint("Successful page directory switch.\n");
}

void paging_init() {
	register_interrupt_handler(14, (isr_t) page_fault);

	tmp_heap = PAGE_ALIGN((uint8_t*) (bitmap + bitmap_size));
	
	page_directory = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
	page_dir_loc = (uint32_t) page_directory - BASE_VIRTUAL;
	memory_set((uint8_t*) page_directory, 0, sizeof(page_directory_t));
	
	uint32_t i, j;
	for(i = 0; i < 0x100000; i += 0x1000) map_virtual_address(page_directory, i, 0); // important this happens first, reserves space in pmm for <1MB grub stuff and identity maps it
	for(j = 0x100000; j < 0x400000; j += 0x1000) map_virtual_address(page_directory, j + BASE_VIRTUAL, j);
	
	asm volatile("bpointdebug:");
	asm volatile("cli");
	load_page_dir(page_dir_loc);
	asm volatile("sti");
}

void page_fault(registers_t regs) {
	uint32_t faulting_address;
	asm volatile ("mov %%cr2, %0" : "=r" (faulting_address));

	int present = !(regs.err_code & 0x1);		// page not present
	int rw = regs.err_code & 0x2;				// write operation?
	int user = regs.err_code & 0x4;				// user mode?
	int reserved = regs.err_code & 0x8;
	int id = regs.err_code & 0x10;				// from instr fetch?

	kprint("PAGE FAULT (");
	if (present) { kprint("present"); }
	if (rw) { kprint(" read-only"); }
	if (user) { kprint(" user-mode"); }
	if (reserved) { kprint(" reserved"); }
	kprint(") at 0x");
	khex(faulting_address);
	kprint("\n");
	kpanic("PAGE FAULT - STOPPING KERNEL\n");
	
	UNUSED(id);
}