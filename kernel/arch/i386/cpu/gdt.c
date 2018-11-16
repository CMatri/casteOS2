#include <string.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>

extern void gdt_load();
extern void tss_flush();
static void write_tss(int32_t, uint16_t, uint32_t);

tss_entry_t tss_entry;
struct gdt gdt_toc;
struct gdt_entry gdt_table[NUM_GDT_ENTRIES];

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
	gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* USER CODE SEG */
	gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* USER DATA SEG */
	
	write_tss(5, 0x10, ((uint32_t) &stack_bottom - (uint32_t) &KERNEL_STACK_SIZE) - 4);

	gdt_toc.base = (unsigned int) &gdt_table;
	gdt_toc.size = (sizeof(struct gdt_entry) * NUM_GDT_ENTRIES) - 1;

	gdt_load();
	tss_flush();
}

static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0) {
 	uintptr_t base = (uintptr_t) &tss_entry;
	uintptr_t limit = base + sizeof(tss_entry);

  	/* Add the TSS descriptor to the GDT */
	gdt_set_entry(num, base, limit, 0xE9, 0x00);

	memset(&tss_entry, 0x0, sizeof(tss_entry));

	tss_entry.ss0 = ss0;
	tss_entry.esp0 = esp0;
	/* Zero out the descriptors */
	tss_entry.cs = 0x00;
	tss_entry.ss = 0x00;
	tss_entry.ds = 0x00;
	tss_entry.es = 0x00;
	tss_entry.fs = 0x00;
	tss_entry.gs = 0x00;
	tss_entry.iomap_base = sizeof(tss_entry);
}