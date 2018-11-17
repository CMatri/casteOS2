#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <kernel/paging.h>

typedef struct {
	uint32_t pid;
	page_directory_t* page_dir;
} process_t;

void create_process(uint8_t* code, uint32_t code_length);

#endif