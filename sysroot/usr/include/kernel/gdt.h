#ifndef GDT_H
#define GDT_H

struct gdt {
	unsigned short size;
	unsigned int base;
} __attribute__((packed));

struct gdt_entry {
	unsigned short limit_low;		/* GDT 0-15: Segment Limit */
	unsigned short base_low;		/* GDT 16-31: Base Address */
	unsigned char base_middle;		/* GDT 32-39: Base Address */
	unsigned char access;			/* GDT 40-47: Access bits */
	unsigned char granularity;		/* GDT 48-55: Segment Limit */
	unsigned char base_high;		/* GDT 56-63: Base Address */
} __attribute__((packed));

void gdt_init();

#endif