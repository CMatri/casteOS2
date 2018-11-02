#include "heap.h"
#include "paging.h"
#include "../libc/common.h"
#include "../kernel/kernel.h"

heap_meta_t *head, *tail;
uint32_t heap_start, heap_end, last_alloc; 

heap_meta_t* get_free_block(uint16_t size) {
	heap_meta_t* curr = head;
	while(curr) {
		if(curr->free && curr->size >= size) return curr;
		curr = curr->next;
	}
	return 0;
}

void* kmalloc(uint16_t size) {
	uint16_t total_size;
	void* block;
	heap_meta_t *header;
	if(!size) return 0;
	
	header = get_free_block(size);
	if(header) {
		header->free = 0;
		return (void*) (header + 1);
	}
	
	total_size = sizeof(heap_meta_t) + size;
	if(last_alloc + total_size > heap_end) {
		map_virtual_address(current_page_directory(), (uint32_t) heap_end, 0);
		heap_end += PAGE_SIZE;
	}
	
	block = last_alloc;
	header = block;
	header->size = size;
	header->free = 0;
	header->next = 0;
	if(!head) head = header;
	if(tail) tail->next = header;
	tail = header;
	last_alloc = (uint32_t) header + total_size;
	return (void*)(header + 1);
}

void heap_init() {
	last_alloc = PAGE_ALIGN((uint32_t) (&kernel_virtual_start) + 8 * M); // kernel heap located 8 MB after kernel.
	heap_start = last_alloc;
	heap_end = heap_start;
	kprint("Kernel heap starts at 0x");
	khex(heap_start);
	kprint("\n");
}