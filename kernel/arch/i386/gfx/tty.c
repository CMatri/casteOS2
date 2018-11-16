#include <string.h>
#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/logger.h>
#include <kernel/vesa_graphics.h>

vbe_info_t* info;
char history[HIST_LENGTH];
font_t cur_font;
int end_hist_idx;
int disp_hist_idx;
int16_t cur_x, cur_y;
uint32_t cur_foreground, cur_background;

void tty_set_vbe_info(vbe_info_t* i) {
	info = i;
}

void tty_write_char(char c) {
	if(end_hist_idx >= HIST_LENGTH) return;
	history[end_hist_idx++] = c;
}

void tty_write_string(char* str) {
	int i;
	for(i = 0; i < strlen(str); i++) tty_write_char(str[i]);
}

void tty_clear_screen() {
	memset(history, 0, HIST_LENGTH * sizeof(char));
}

void tty_write_backspace() {

}

void tty_draw_char(char c) {
	uint16_t cx, cy;
	uint16_t mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

	for(cy = 0; cy < 12; cy++) {
		for(cx = 0; cx < 8; cx++) {
			vesa_put_pixel(0, (cur_x * cur_font.char_width) + cx, (cur_y * cur_font.char_height) + cy, /*cur_font.data[c][cy]*/Terminess_Powerline_Bold[c][cy] & mask[7 - cx] ? cur_foreground : cur_background);
		}
	}
}

void tty_draw() {
	int i;
	cur_x = 0;
	cur_y = 0;
	cur_foreground = 0xFFFFFFFF;
	cur_background = 0;

	for(i = disp_hist_idx; i < end_hist_idx; i++) {
		char c = history[i];
		if(c == '\n') {
			cur_y++;
			cur_x = 0;
			continue;
		}

		tty_draw_char(c);
		if(++cur_x >= info->Xres / cur_font.char_width) {
			cur_x = 0;
			cur_y++;
		}

		if(cur_y >= info->Yres / cur_font.char_height) break;
	}
}

void tty_init() {
	tty_clear_screen();
	tty_set_vbe_info(vesa_get_vbe_info());

	// set up a default font
	//cur_font.data = &Terminess_Powerline_Bold;
	cur_font.char_width = 8;
	cur_font.char_height = 12;
	
	end_hist_idx = 0;
	disp_hist_idx = 0;
}