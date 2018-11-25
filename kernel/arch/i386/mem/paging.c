// TODO: CLEAN UP THIS GOD AWFUL CODE IT MAKES MY EYES BLEED

#include <kernel/multiboot.h>
#include <kernel/isr.h>
#include <kernel/pmm.h>
#include <kernel/isr.h>
#include <kernel/heap.h>
#include <kernel/paging.h>
#include <string.h>
#include <kernel/common.h>
#include <kernel/kernel.h>
#include <kernel/logger.h>
#include <kernel/screen.h>

static page_directory_t* cur_page_directory = 0;
static uint32_t page_dir_loc = 0;
page_directory_t* kpage_dir = 0;
int paging_enabled = 0;

void flush_tlb() {
	asm volatile("mov %cr3, %eax");
	asm volatile("mov %eax, %cr3");
}

void* dumb_kmalloc_a(uint32_t size) {
	void* ret = tmp_heap;
	if(!IS_ALIGN(ret)) ret = (void*) PAGE_ALIGN(ret);
	tmp_heap += size;
	if(tmp_heap >= tmp_heap_end) klog("OOPS BITCH TIME TO ACTUALLY FIX YOUR CODE!\n");
	return ret;
}

page_directory_t* current_page_directory() {
	return cur_page_directory;
}

uint32_t virtual2phys(page_directory_t* dir, uint32_t virtual_addr) {
    if(!paging_enabled) return (uint32_t) virtual_addr - BASE_VIRTUAL;

    uint32_t page_dir_idx = (uint32_t) (virtual_addr >> 22);
    uint32_t page_tbl_idx = (uint32_t) (virtual_addr >> 12) & 0x3FF;
    uint32_t page_frame_offset = virtual_addr & 0xFFF;
    
    if(!dir->ref_tables[page_dir_idx]) {
        klog("virtual2phys: page dir entry does not exist\n");
        return 0;
    }

    page_table_t* table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        klog("virtual2phys: page table entry does not exist\n");
        return 0;
    }

    uint32_t t = table->pages[page_tbl_idx].frame;
    t = (uint32_t) (t << 12) + page_frame_offset;
    return t;
}


void log_page_directory(page_directory_t* dir, int tables) {
	klog("PAGE DIRECTORY (0x");
	klhex((uint32_t) dir);
	klog(") -> tables { ");
	int i;
	for(i = 0; i < 1024; i++) { 
		klhex(dir->tables[i].rep); 
		klog(", "); 
	}

	klog(" }\nref_tables { ");
	for(i = 0; i < 1024; i++) { klhex((uint32_t) dir->ref_tables[i]); klog(", "); }
	klog(" }\n");
}

void copy_page_directory(page_directory_t* dst, page_directory_t* src) {
    uint32_t i;
    for(i = 0; i < 1024; i++) {
        if(kpage_dir->ref_tables[i] == src->ref_tables[i]) {
            // Link kernel pages
            dst->tables[i] = src->tables[i];
            dst->ref_tables[i] = src->ref_tables[i];
        } else {
            // For non-kernel pages, copy the pages (for example, when forking process, you don't want the parent process mess with child process's memory)
            dst->ref_tables[i] = copy_page_table(src, dst, i, src->ref_tables[i]);
            uint32_t phys = (uint32_t) virtual2phys(src, (uint32_t) dst->ref_tables[i]);
            dst->tables[i].frame = phys >> 12;
            dst->tables[i].user = 1;
            dst->tables[i].rw = 1;
            dst->tables[i].present = 1;
        }
    }
}

page_table_t* copy_page_table(page_directory_t* src_page_dir, page_directory_t* dst_page_dir, uint32_t page_dir_idx, page_table_t* src) {
    page_table_t * table = (page_table_t*) kmalloc(sizeof(page_table_t), 1);
    int i;
    for(i = 0; i < 1024; i++) {
        if(!table->pages[i].frame) continue;
        uint32_t src_virtual_address = (page_dir_idx << 22) | (i << 12) | (0);
        uint32_t dst_virtual_address = src_virtual_address;
        uint32_t tmp_virtual_address = 0;

        map_virtual_address(dst_page_dir, dst_virtual_address, -1, 1);
        map_virtual_address(src_page_dir, tmp_virtual_address, (uint32_t)virtual2phys(dst_page_dir, (void*)dst_virtual_address), 1);
        if (src->pages[i].present) table->pages[i].present = 1;
        if (src->pages[i].rw)      table->pages[i].rw = 1;
        if (src->pages[i].user)    table->pages[i].user = 1;
        if (src->pages[i].accessed)table->pages[i].accessed = 1;
        if (src->pages[i].dirty)   table->pages[i].dirty = 1;
        memcpy((void*)tmp_virtual_address, (void*)src_virtual_address, PAGE_SIZE);
        free_page(src_page_dir, tmp_virtual_address, 0);
    }

    return table;
}

void map_virtual_address_space(page_directory_t* dir, uint32_t vaddr, uint32_t paddr, int u, uint32_t length) {
	uint32_t loc = 0;
	while(loc < length) {
		map_virtual_address(dir, vaddr + loc, paddr != 0xFFFFFFFF ? paddr + loc : -1, u);
		loc += PAGE_SIZE;
	}
}

void map_virtual_address(page_directory_t* dir, uint32_t vaddr, uint32_t paddr, int u) {
	if(vaddr % PAGE_SIZE != 0 || (paddr % PAGE_SIZE != 0 && paddr != 0xFFFFFFFF))  {
		klog("Vaddr: 0x");
		klhex(vaddr);
		klog(" Paddr: 0x");
		klhex(paddr);
		kpanic("\nMapping misaligned addresses\n");
	}

	uint32_t pd_idx = (uint32_t) vaddr >> 22;
	uint32_t pt_idx = (uint32_t) (vaddr >> 12) & 0x3FF;
	
	if(!dir) {
		kpanic("Attempt to map page to NULL directory.\n");
		return;
	}
	
	if(!dir->ref_tables[pd_idx]) {		
		page_table_t* t = dumb_kmalloc_a(sizeof(page_table_t));
		memset((uint8_t*) t, 0, sizeof(page_table_t));
		uint32_t t_paddr = (uint32_t) t - BASE_VIRTUAL;
		
		dir->tables[pd_idx].present = 1;
		dir->tables[pd_idx].user = u;
		dir->tables[pd_idx].rw = 1;
		dir->tables[pd_idx].page_size = 0;
		dir->tables[pd_idx].frame = t_paddr >> 12;
		dir->ref_tables[pd_idx] = t;
	}
	
	page_table_t* table = dir->ref_tables[pd_idx];
	if(!table->pages[pt_idx].present) {
		uint32_t addr = paddr;
		if(paddr == 0xFFFFFFFF) addr = allocate_block();
		else set_block(addr);
		
		table->pages[pt_idx].present = 1;
		table->pages[pt_idx].rw = 1;
		table->pages[pt_idx].user = u;
		table->pages[pt_idx].frame = addr >> 12;		
	}

	flush_tlb();
}

void free_page(page_directory_t* dir, uint32_t virtual_addr, int free) {
    uint32_t page_dir_idx = (uint32_t) (virtual_addr >> 22);
    uint32_t page_tbl_idx = (uint32_t) (virtual_addr >> 12) & 0x3FF;
    
    if(!dir->ref_tables[page_dir_idx]) {
        klog("free_page: page dir entry does not exist\n");
        return;
    }
    
    page_table_t* table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        klog("free_page: page table entry does not exist\n");
        return;
    }

    // The table entry is found !
    if(free) free_block(table->pages[page_tbl_idx].frame);
    table->pages[page_tbl_idx].present = 0;
    table->pages[page_tbl_idx].frame = 0;
}

void load_page_dir(uint32_t dir) {
	klog("PAGE_DIR: ");
	klhex((uint32_t) dir);
	klog("\n");
	
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
	kpage_dir = (page_directory_t*) dumb_kmalloc_a(sizeof(page_directory_t));
	cur_page_directory = kpage_dir;
	page_dir_loc = (uint32_t) kpage_dir - BASE_VIRTUAL;
	memset((uint8_t*) kpage_dir, 0, sizeof(page_directory_t));
	
	uint32_t vaddr = BASE_VIRTUAL;
	uint32_t paddr = 0;
	for(; paddr < 32 * 1024 * 1024; paddr += PAGE_SIZE, vaddr += PAGE_SIZE) map_virtual_address(kpage_dir, vaddr, paddr, 1); // map first 32 MB of memory, this is important. Also reserves space in the pmm for kernel and GRUB.
	
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
	if (id) { kprint(" id"); klog(" id"); }
	kprint(") at 0x");
	klog(") at 0x");
	khex(faulting_address);
	klhex(faulting_address);
	kprint("\n");
	klog("\n");
	kpanic("PAGE FAULT - STOPPING KERNEL\n");
	
	for(;;) { asm volatile("hlt"); }
}

void* ksbrk(uint16_t size) { // TODO: Implement for size < 0
	uint32_t old_heap_curr;
	restart_sbrk:
	old_heap_curr = heap_curr;
	if(!size) { klog("Trying to misuse ksbrk: size < 0\n"); return (void*) heap_curr; }
	else if(size > 0) {
		uint32_t new_boundary = heap_curr + size;
		if(new_boundary < heap_end) { heap_curr = new_boundary; return (void*) old_heap_curr; }
		else if(new_boundary >= heap_start + heap_max) { kpanic("KSBRK OUT OF SPACE\n"); return 0; }
		else if(new_boundary >= heap_end) {
			uint32_t runner = heap_end;
			while(runner < new_boundary) {
				map_virtual_address(kpage_dir, runner, -1, 1);
				runner += PAGE_SIZE;
			} 
			if(old_heap_curr != heap_curr) goto restart_sbrk;
			heap_end = runner;
			heap_curr = new_boundary;			
			goto ret;
		}
	}
	ret:
	return (void*) old_heap_curr;
}