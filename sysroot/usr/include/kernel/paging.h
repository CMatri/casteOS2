#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <kernel/isr.h>

#define IS_ALIGN(addr) ((((uint32_t)(addr)) | 0xFFFFF000) == 0)
#define PAGE_ALIGN(addr) ((((uint32_t)(addr)) & 0xFFFFF000) + 0x1000)
#define SET_PGBIT(cr0) (cr0 = cr0 | 0x80000000)
#define CLEAR_PSEBIT(cr4) (cr4 = cr4 & 0xffffffef)
#define PAGE_SIZE 0x1000
#define TMP_PAGE_0               (void*) 0xFFBFB000
#define TMP_PAGE_1               (void*) 0xFFBFC000
#define TMP_PAGE_2               (void*) 0xFFBFC000
#define TMP_PAGE_3               (void*) 0xFFBFC000
#define TMP_PAGE_4               (void*) 0xFFBFF000

typedef union page_dir_entry {
    struct {
        unsigned int present    : 1;
        unsigned int rw         : 1;
        unsigned int user       : 1;
        unsigned int w_through  : 1;
        unsigned int cache      : 1;
        unsigned int access     : 1;
        unsigned int reserved   : 1;
        unsigned int page_size  : 1;
        unsigned int global     : 1;
        unsigned int available  : 3;
        unsigned int frame      : 20;
    };

    uint32_t rep;
} page_dir_entry_t;

typedef struct page_table_entry {
    unsigned int present    : 1;
    unsigned int rw         : 1;
    unsigned int user       : 1;
    unsigned int reserved   : 2;
    unsigned int accessed   : 1;
    unsigned int dirty      : 1;
    unsigned int reserved2  : 2;
    unsigned int available  : 3;
    unsigned int frame      : 20;
} page_table_entry_t;


typedef struct page_table {
    page_table_entry_t pages[1024];
} page_table_t;

typedef struct page_directory {
    // The actual page directory entries(note that the frame number it stores is physical address)
    page_dir_entry_t tables[1024];
    // We need a table that contains virtual address, so that we can actually get to the tables
    page_table_t * ref_tables[1024];
} page_directory_t;

extern void load_page_directory(unsigned int*);
extern void enable_paging();
extern uint32_t BootPageDirectory;
extern int paging_enabled;
extern page_directory_t* kpage_dir;
void paging_init();
uint32_t virtual2phys(page_directory_t* dir, uint32_t virtual_addr);
void log_page_directory(page_directory_t* dir, int tables);
void copy_page_directory(page_directory_t* dst, page_directory_t* src);
page_table_t* copy_page_table(page_directory_t* src_page_dir, page_directory_t* dst_page_dir, uint32_t page_dir_idx, page_table_t* src);
void free_page(page_directory_t* dir, uint32_t virtual_addr, int free);
void page_fault(registers_t regs);
void map_virtual_address(page_directory_t* dir, uint32_t vaddr, uint32_t paddr, int u);
void map_virtual_address_space(page_directory_t* dir, uint32_t vaddr, uint32_t paddr, int u, uint32_t length);
void* ksbrk(uint16_t size);
page_directory_t* current_page_directory();  
uint8_t* tmp_heap;
uint8_t* tmp_heap_end;

#endif