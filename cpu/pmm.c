#include "pmm.h"
#include "paging.h"
#include "multiboot.h"
#include "../libc/mem.h"
#include "../kernel/kernel.h"
#include "../drivers/screen.h"

void pmm_init(uint32_t mem_size) {
	if(mem_size % BLOCK_SIZE != 0) kprint("Memory size given to PMM not a multiple of 4096! Things may get weird.\n");
	bitmap = (uint8_t*) (mmap.entries + mmap.length * sizeof(mboot_memmap_t));
	total_blocks = mem_size / BLOCK_SIZE;
	bitmap_size = total_blocks / BITSIZE;
	if(bitmap_size * BITSIZE < total_blocks)
        bitmap_size++;
	
	memory_set(bitmap, 0, bitmap_size);
}

uint32_t allocate_block() { // returns physical address of first free block
	uint32_t block = first_free_block();
	bitmap[block / BLOCK_SIZE / BITSIZE] |= (uint8_t) (0x1 << ((block / BLOCK_SIZE) % BITSIZE));
	return block;
}

void set_block(uint32_t addr) { // takes an address sets bit for addr in bitmap
	if(addr % 0x1000 != 0) {
		kprint("Trying to set bitmap with misaligned address: ");
		kprint("0x");
		khex(addr);
		kprint("\n");
		return;
	}
	
	SETBIT(addr / BLOCK_SIZE);
}

void free_block(uint32_t i) {
	CLEARBIT(i);
}

uint32_t first_free_block() { // returns physical address of first free block in memory
	uint32_t i, j;
	for(i = 0; i < total_blocks / BITSIZE; i++) {
		if(bitmap[i] != 0xFF) {
			for(j = 0; j < BITSIZE; j++) {
				if(!(bitmap[i] & (1 << j))) return (i * BITSIZE + j) * BLOCK_SIZE;
			}
		}
	}
	
	kpanic("Ran out of blocks!\n");
	return (uint32_t) -1;
}