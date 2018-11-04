#include <kernel/devices/timer.h>
#include <kernel/cpu/isr.h>
#include <kernel/sys/ports.h>
#include <kernel/io/screen.h>
#include <libc/common.h>

uint32_t tick = 0;

static void timer_callback(registers_t *regs) {
	tick++;
	UNUSED(regs);
}

void init_timer(uint32_t freq) {
	register_interrupt_handler(IRQ0, timer_callback);
	
	 /* Get the PIT value: hardware clock at 1193180 Hz */
	uint32_t divisor = 11932180 / freq;
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	port_byte_out(0x42, 0x36);
	port_byte_out(0x40, low);
	port_byte_out(0x40, high);
}

void timer_wait(int ticks) {
	unsigned long eticks;
	eticks = tick + ticks;
	while(tick < eticks);
}