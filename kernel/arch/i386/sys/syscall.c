#include <kernel/syscall.h>
#include <kernel/isr.h>
#include <kernel/screen.h>

void syscall_callback(registers_t *regs) {
	int call_id = regs->eax;

	switch(call_id) {
		case 0x0: // TODO: exit
			//for(;;) {}
			break;
		case 0x1:
			kprint_char(regs->ebx);
			break;
	}
}

void init_syscall() {
    register_interrupt_handler(0x70, syscall_callback);
}