[extern gdt_toc]
[global gdt_load]

gdt_load:
	lgdt [gdt_toc]
	mov ax, 0x10 ; Reload data segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
	jmp 0x08:update_registers
	
update_registers:
	ret
	