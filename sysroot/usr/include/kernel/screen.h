#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_ADDRESS 0xC00B8000 // 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4
#define RED_ON_BLACK 0x4f

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#include <stdint.h>

/* Public kernel API */
void clear_screen();
void khex(uint32_t n);
void kdec(uint32_t n);
void klhex(uint32_t n);
void kldec(uint32_t n);
void kprint_base(uint32_t n, uint32_t b, int log);
void kprint_at_color(char *message, int col, int row, int color);
void kprint_at(char *message, int col, int row);
void kprint(char *message);
void kprint_backspace();
void kpanic(char *message);
int print_char(char c, int col, int row, char attr);

#endif
