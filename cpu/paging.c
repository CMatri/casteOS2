#include "paging.h"

void paging_init() {
	int i;
	for(i = 0; i < 1024; i++) page_directory[i] = 0x00000002;
	unsigned int j;
	for(j = 0; j < 1024; j++) first_page_table[j] = (j * 0x1000) | 3;
	page_directory[0] = ((unsigned int) first_page_table) | 3;

	load_page_directory((unsigned int*) page_directory);
	enable_paging();
}