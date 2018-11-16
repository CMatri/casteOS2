#ifndef VGA_H
#define VGA_H

#define VGA_VIDEO_ADDRESS 0xC00B8000 // 0xb8000
#define VGA_MAX_ROWS 25
#define VGA_MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4
#define RED_ON_BLACK 0x4f

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#include <stdint.h>

/* Public kernel API */
void vga_clear_screen();
void vga_print_at_color(char *message, int col, int row, int color);
void vga_print_at(char *message, int col, int row);
void vga_print(char *message);
void vga_print_char(char c);
void vga_print_backspace();

#endif
