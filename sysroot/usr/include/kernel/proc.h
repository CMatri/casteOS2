#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <kernel/paging.h>

typedef struct process {
	uint32_t pid;
	struct process* next;
	page_directory_t* page_dir;
} process_t;

void task_switch();
void create_process(uint8_t* code, uint32_t code_length);

#endif