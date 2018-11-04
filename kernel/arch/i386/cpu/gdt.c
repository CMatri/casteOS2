#include <kernel/gdt.h>

extern void gdt_load();

struct gdt gdt_toc;
struct gdt_entry gdt_table[3];

static void gdt_set_entry(int sid, unsigned int base, unsigned int limit, unsigned char access, unsigned char granularity)
{
	gdt_table[sid].base_low = base & 0xFFFF;
	gdt_table[sid].base_middle = (base >> 16) & 0xFF;
	gdt_table[sid].base_high = (base >> 24) & 0xFF;

	gdt_table[sid].limit_low = limit & 0xFFFF;
	gdt_table[sid].granularity = (limit >> 16) & 0x0F;

	gdt_table[sid].access = access;
	gdt_table[sid].granularity |= (granularity & 0xF0);
}

void gdt_init()
{
	gdt_set_entry(0, 0, 0, 0, 0);
	gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* 0x9A = CODEDATA_BIT ON. */
	gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* 0x92 = CODEDATA_BIT OFF.*/

	gdt_toc.base = (unsigned int) &gdt_table;
	gdt_toc.size = (sizeof(struct gdt_entry) * 3) - 1;

	gdt_load();
}