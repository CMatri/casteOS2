#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

typedef struct heap_meta {
	uint16_t size;
	uint8_t free;
	struct heap_meta* next;
} heap_meta_t;

struct heap_meta* get_free_block(uint16_t size);
void* kmalloc(uint16_t size);
void heap_init();
extern heap_meta_t *head, *tail;
extern uint32_t heap_start, heap_end, heap_curr, heap_max; 

#endif