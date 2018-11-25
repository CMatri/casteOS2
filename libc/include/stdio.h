#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>

#define STDIO_SIZE 4096

#ifdef __cplusplus
extern "C" {
#endif

char stdin[STDIO_SIZE];
volatile int in_size;

void print(const char* data, short data_length);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
unsigned char inportb(unsigned short);
unsigned char inportw(unsigned short);
void outportb(unsigned short, unsigned char);
int getchar(); // get a char and print it
int getch(); // get a char and don't print it
char* getstr(char *str);
void init_stdio();

#ifdef __cplusplus
}
#endif

#endif
