#include "pmm.h"
#include "../kernel/kernel.h"

uint8_t *bitmap = (uint8_t*)(&kernel_virtual_end);
uint8_t *mem_start;
uint32_t total_blocks;
uint32_t bitmap_size;

void pmm_init(uint32_t mem_size) { // In MB
	uint64_t mem_size_bytes = mem_size * 1024 * 1024;
	if(mem_size_bytes % BLOCK_SIZE != 0) {
		kpanic("Memory size given to PMM not a multiple of 4096! Things may get weird.\n");
	}
	
	total_blocks = mem_size_bytes / BLOCK_SIZE;
	kprint("total blocks: ");
	kprint("\n");
}