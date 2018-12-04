#include <kernel/syscall.h>
#include <kernel/isr.h>
#include <kernel/screen.h>
#include <kernel/keyboard.h>

void syscall_callback(registers_t *regs) {
	int call_id = regs->eax;
	switch(call_id) {
		case 0x0: // TODO: restart
			break;
		case 0x1: // TODO: exit
			break;
		case 0x2: // TODO: fork
			break;
		case 0x3: // read
			switch(regs->ebx) {
				case 0: // stdin
					; // needed bc C doesn't allow declarations after labels, only statements
					int c = keyboard_get_key();
					klog("Byte: ");
					klhex(c);
					asm volatile("movl %%ebx, %%eax": :"b"(c));
					break;
				case 1: // stdout
					break;
				case 2: // stderr
					break;
				default:
					break;
			}
			break;
		case 0x4: // write
			kprint_char(regs->ebx);
		case 0x5: // TODO: open
			break;
	}
}

void init_syscall() {
    register_interrupt_handler(0x70, syscall_callback);
}