/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Resolve references relative to 0xC010000. Relocation specifies that non-relative memory-references should should use the relocation address as base in address calculations. */
	. = 0xC0100000;
	kernel_virtual_start = .;
    kernel_physical_start = . - 0xC0000000;
	/* First we'll put the .text section. BLOCK and ALIGN synonymous, BLOCK is for compatibility with old linker scripts. AT to tell GRUB to locate the kernel at 1M in physical memory */
	.text ALIGN(4K) : AT(ADDR(.text)-0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata)-0xC0000000)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data)-0xC0000000)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss)-0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}
	kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}