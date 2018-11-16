#include <kernel/screen.h>
#include <kernel/vesa_graphics.h>
#include <kernel/vga_graphics.h>
#include <kernel/tty.h>
#include <kernel/common.h>
#include <kernel/logger.h>
#include <string.h>

void khex(uint32_t n) {
	kprint_base(n, 16, 0);
}

void kdec(uint32_t n) {
	kprint_base(n, 10, 0);
}

void klhex(uint32_t n) {
	kprint_base(n, 16, 1);
}

void kldec(uint32_t n) {	
	kprint_base(n, 10, 1);
}

void kprint_base(uint32_t n, uint32_t b, int log) {
	uint32_t baseNum;

	if(n > (b - 1)) {
		kprint_base(n / b, b, log);
		baseNum = n % b;
	} else baseNum = n;

	
	if(baseNum > 9) { 
        if(!log) {
            #ifdef VESA_GRAPHICS
                tty_write_char(baseNum + 65 - 10);
            #else
                vga_print_char(baseNum + 65 - 10);
            #endif 
        } else klog_byte(baseNum + 65 - 10); 
    } else { 
        if(!log) { 
            #ifdef VESA_GRAPHICS
                tty_write_char(baseNum + 0x30);
            #else
                vga_print_char(baseNum + 0x30);
            #endif 
        } else klog_byte(baseNum + 0x30); 
    }
}

void kprint(char *message) {
    #ifdef VESA_GRAPHICS
        tty_write_string(message);
    #else
        vga_print(message);
    #endif 
}

void kprint_backspace() {
    #ifdef VESA_GRAPHICS
        tty_write_backspace();
    #else
        vga_print_backspace();
    #endif
}

void kpanic(char *message) {
    klog(message);

    #ifdef VESA_GRAPHICS
        tty_write_string(message);
    #else
        vga_print_at_color(message, -1, -1, RED_ON_BLACK);
    #endif

	asm("hlt");
}

void kprint_char(char c) {
    #ifdef VESA_GRAPHICS
        tty_write_char(c);
    #else
        vga_print_char(c);
    #endif
}

void kclear_screen() {
    #ifdef VESA_GRAPHICS
        tty_clear_screen();
    #else
        vga_clear_screen();
    #endif
}