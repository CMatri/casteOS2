#include <kernel/misc/multiboot.h>
#include <kernel/cpu/isr.h>
#include <kernel/mem/pmm.h>
#include <kernel/cpu/isr.h>
#include <kernel/mem/heap.h>
#include <kernel/mem/paging.h>
#include <libc/mem.h>
#include <libc/common.h>
#include <kernel/kernel.h>
#include <kernel/io/logger.h>
#include <kernel/io/screen.h>

static page_directory_t* page_directory = 0;
static uint32_t page_dir_loc = 0;
int paging_enabled = 0;

void flush_tlb() {
	asm volatile("mov %cr3, %eax");
	asm volatile("mov %eax, %cr3");
}

void* dumb_kmalloc_a(uint32_t size) {
	void* ret = tmp_heap;
	if(!IS_ALIGN(ret)) ret = (void*) PAGE_ALIGN(ret);
	tmp_heap += size;
	return ret;
}

page_directory_t* current_page_directory() {
	return page_directory;
}

void map_virtual_address_space(page_directory_t* dir, uint32_t vaddr, uint32_t paddr, uint32_t length) {
	uint32_t loc = 0;
	while(loc < length) {
		map_virtual_address(dir, vaddr + loc, paddr + loc);
		loc += PAGE_SIZE;
	}
}

void map_virtual_address(page_directory_t* dir, uint32_t vaddr, uint32_t paddr) {
	if(vaddr % PAGE_SIZE != 0 || paddr % PAGE_SIZE != 0) kpanic("Mapping misaligned addresses\n");
	uint32_t pd_idx = (uint32_t) vaddr >> 22;
	uint32_t pt_idx = (uint32_t) (vaddr >> 12) & 0x03FF;
	
	if(!dir) {
		kpanic("Attempt to map page to NULL directory.\n");
		return;
	}
	
	if(!dir->ref_tables[pd_idx]) {		
		page_table_t* t = dumb_kmalloc_a(sizeof(page_table_t));
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
		uint32_t addr = paddr;
		if(!paddr) addr = allocate_block();
		else set_block(addr);
		table->pages[pt_idx].present = 1;
		table->pages[pt_idx].rw = 1;
		table->pages[pt_idx].user = 1;
		table->pages[pt_idx].frame = addr >> 12;		
	}

	flush_tlb();
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
}

void paging_init() {
	register_interrupt_handler(14, (isr_t) page_fault);

	tmp_heap = (uint8_t*) PAGE_ALIGN((uint8_t*) (bitmap + bitmap_size));
	tmp_heap_end = (uint8_t*) (tmp_heap + 1024 * 1024 * 4); // 4 MB for page frames
	page_directory = (page_directory_t*) dumb_kmalloc_a(sizeof(page_directory_t));
	page_dir_loc = (uint32_t) page_directory - BASE_VIRTUAL;
	memory_set((uint8_t*) page_directory, 0, sizeof(page_directory_t));
	
	uint32_t vaddr = BASE_VIRTUAL;
	uint32_t paddr = 0;
	for(; paddr < 32 * 1024 * 1024; paddr += PAGE_SIZE, vaddr += PAGE_SIZE) map_virtual_address(page_directory, vaddr, paddr); // map first 32 MB of memory, this is important. Also reserves space in the pmm for kernel and GRUB.
	
	asm volatile("cli"); // disable interrupts before setting cr3 to page directory so IRQs and ISRs don't get messed up
	load_page_dir(page_dir_loc);
	asm volatile("sti");

	flush_tlb();
	paging_enabled = 1;
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
	klog("PAGE FAULT (");
	if (present) { kprint("present"); klog("present"); }
	if (rw) { kprint(" read-only"); klog(" read-only"); }
	if (user) { kprint(" user-mode"); klog(" user-mode"); }
	if (reserved) { kprint(" reserved"); klog(" reserved"); }
	kprint(") at 0x");
	klog(") at 0x");
	khex(faulting_address);
	klhex(faulting_address);
	kprint("\n");
	klog("\n");
	kpanic("PAGE FAULT - STOPPING KERNEL\n");
	
	UNUSED(id);
	for(;;) { asm volatile("hlt"); }
}

void* ksbrk(uint16_t size) { // TODO: Implement for size < 0
	uint32_t old_heap_curr = heap_curr;
	if(!size) return (void*) heap_curr;
	else if(size > 0) {
		uint32_t new_boundary = heap_curr + size;
		if(new_boundary <= heap_end) { heap_curr = new_boundary; return (void*) old_heap_curr; }
		else if(new_boundary > heap_start + heap_max) return 0;
		else if(new_boundary > heap_end) {
			uint32_t runner = heap_end;
			while(runner < new_boundary) {
				map_virtual_address(page_directory, runner, 0);
				runner += PAGE_SIZE;
			} 
			// potential bug check impl here: https://github.com/szhou42/osdev/blob/master/src/kernel/mem/paging.c
			heap_end = runner;
			heap_curr = new_boundary;
			return (void*) old_heap_curr;
		}
	}
	return (void*) old_heap_curr;
}