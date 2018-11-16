#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <kernel/isr.h>
#include <stdint.h>
#include <stdio.h>

#define KB_BUF_SIZE 20
#define MAX_HOOKS 100

void init_keyboard();
void keyboard_handler(registers_t *regs);

enum kbOpts
{
  LSHIFT = 0x01,
  CTRL = 0x02,
  LALT = 0x04,
  CAPS_LOCK = 0x08,
  RALT = 0x10,
  RSHIFT = 0x20
};

struct kb_hook_struct
{
  void* func;
  int key;
} __attribute((packed));
typedef struct kb_hook_struct kb_hook_t;

#endif