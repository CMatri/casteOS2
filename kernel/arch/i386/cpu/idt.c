#include <kernel/idt.h>

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = (uint16_t)((handler) & 0xFFFF);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0xEE; // 0x8E 
    idt[n].high_offset = (uint16_t)(((handler) >> 16) & 0xFFFF);
}

void set_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    asm volatile("lidt (%0)" : : "r" (&idt_reg));
}
