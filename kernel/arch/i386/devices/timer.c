#include <kernel/timer.h>
#include <kernel/isr.h>
#include <kernel/ports.h>
#include <kernel/screen.h>
#include <kernel/common.h>
#include <kernel/vesa_graphics.h>
#include <kernel/kernel.h>

uint32_t tick = 0;

static void timer_callback(registers_t *regs) {
	tick++;
	
	#ifdef VESA_GRAPHICS 
	if(tick % 2 == 0 && FINISHED_INIT) vesa_update_graphics(); // temporary lol
	#endif
	
	UNUSED(regs);
} 

void init_timer(uint32_t freq) {
	register_interrupt_handler(IRQ0, timer_callback);
	
	 /* Get the PIT value: hardware clock at 1193180 Hz */
	uint32_t divisor = 11932180 / freq;
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	port_byte_out(0x43, 0x36); // Mode 3 (Square wave) and RW low and high (least followed by most significant byte)
	port_byte_out(0x40, low);
	port_byte_out(0x40, high);
}

void timer_wait(int ticks) {
	unsigned long eticks;
	eticks = tick + ticks;
	while(tick < eticks);
}