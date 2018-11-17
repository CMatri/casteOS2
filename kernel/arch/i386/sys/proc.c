#include <kernel/proc.h>
#include <kernel/heap.h>
#include <kernel/paging.h>
#include <string.h>
//https://github.com/cstack/osdev/blob/8681607dc6e816e90eb5fe103540ad42bd1c161e/process.c

int cur_pid = 0;

void create_process(uint8_t* code, uint32_t code_length) {
	process_t* proc = (process_t*) kmalloc(sizeof(process_t), 1);
	proc->pid = cur_pid++;
	proc->page_dir = (page_directory_t*) kmalloc(sizeof(page_directory_t), 1);
	memset(proc->page_dir, 0, sizeof(page_directory_t));
	copy_page_directory(proc->page_dir, kpage_dir);
	map_virtual_address(proc->page_dir, 0, -1); 						// .text
	map_virtual_address(proc->page_dir, PAGE_ALIGN(code_length), -1); 	// stack
}