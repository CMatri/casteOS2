#ifndef TTY_H
#define TTY_H
// assumes 32 bpp for now, should update to support variable bpp

#include <stdint.h>
#include <kernel/multiboot.h>

#define HIST_LENGTH 1000

typedef struct {
	int char_width;
	int char_height;
	uint8_t** data;
} font_t;

void tty_set_vbe_info(vbe_info_t* info);
void tty_write_char(char c);
void tty_write_string(char* str);
void tty_write_backspace();
void tty_clear_screen();
void tty_draw();
void tty_init();

#endif