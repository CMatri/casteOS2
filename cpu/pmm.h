#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define BLOCK_SIZE 4096
#define BITSIZE 8
#define NTH_BIT(N, B) (B & (1 << N)) >> N
#define SETBIT(i) bitmap[i / BITSIZE] = bitmap[i / BITSIZE] | (1 << (i % BITSIZE))
#define CLEARBIT(i) bitmap[i / BITSIZE] = bitmap[i / BITSIZE] & (~(1 << (i % BITSIZE)))
#define ISSET(i) ((bitmap[i / BITSIZE] >> (i % BITSIZE)) & 0x1)

uint32_t allocate_block();
void set_block(uint32_t addr);
void free_block(uint32_t i);
uint32_t first_free_block();
void pmm_init(uint32_t mem_size);

uint8_t *bitmap;
uint32_t total_blocks;
uint32_t bitmap_size;

#endif
