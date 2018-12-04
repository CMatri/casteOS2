#include <kernel/keyboard.h>
#include <kernel/common.h>

static unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0xC0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0x15,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0x25,				/* Right shift */
  '*',
    0xAA,	/* Alt */
  ' ',	/* Space bar */
    0xCA,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0xA1,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0xA2,	/* Left Arrow */
    0,
    0xA3,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0xA4,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

static unsigned char kbdusShift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',     /* Tab */
  'Q', 'W', 'E', 'R', /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '\"', '~',   0,    /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', '<', '>', '?',   0,        /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

uint16_t kb_flags = 0;
unsigned int kb_buf[KB_BUF_SIZE];
kb_hook_t kb_hooks[MAX_HOOKS];
int num_of_hooks = 0;
int last = 0;

void scan_key_hooks(int key) {
	int i;
	for(i = 0; i < num_of_hooks; i++) {
		if(key == kb_hooks[i].key) {
			void (*hook_func)(void);
			hook_func = kb_hooks[i].func;
			(*hook_func)();
		}
	}
}

void add_key_hook(int key, void* func) {
	if(num_of_hooks < MAX_HOOKS) {
		kb_hooks[num_of_hooks].key = key;
		kb_hooks[num_of_hooks].func = func;
		num_of_hooks++;
	} else {
		puts("Too many keyboard hooks assigned...");
	}
}

void read_kb_buf(uint8_t *buf, uint16_t size) {
	((uint8_t*) stdin)[in_size] = kb_buf[size - 1];
}

unsigned int keyboard_get_key() {
	unsigned int c = kb_buf[0];
	int i;
	if(last == 0) return 0;
	last--;
	for(i = 0; i < KB_BUF_SIZE - 1; i++) kb_buf[i] = kb_buf[i + 1];
	return c;
}

void keyboard_handler(registers_t *regs) {
	unsigned char scancode = inportb(0x60);

	if(scancode > 57) return; // TEMP
	if(scancode & 0x80) {
		if(kbdus[scancode & ~0x80] == 0xCA) {
			if(kb_flags & CAPS_LOCK) kb_flags &= ~CAPS_LOCK;
			else kb_flags |= CAPS_LOCK;
		}

		if(kbdus[scancode & ~0x80] == 0xC0) kb_flags &= ~CTRL;
		if(kbdus[scancode & ~0x80] == 0x15)	kb_flags &= ~LSHIFT;
		if(kbdus[scancode & ~0x80] == 0x25)	kb_flags &= ~RSHIFT;
		if(kbdus[scancode & ~0x80] == 0xAA)	kb_flags &= ~LALT;
	} else {
		switch(kbdus[scancode]) {
			case 0xC0:
				kb_flags |= CTRL;
				return;
			case 0x15:
				kb_flags |= LSHIFT;
				return;
			case 0x25:
				kb_flags |= RSHIFT;
				return;
			case 0xAA:
				kb_flags |= LALT;
				return;
			case 0xA1:
				scan_key_hooks(0xA1);
				return;
			case 0xA2:
				scan_key_hooks(0xA2);
				return;
			case 0xA3:
				scan_key_hooks(0xA3);
				return;
			case 0xA4:
				scan_key_hooks(0xA4);
				return;
		}

		if(!(kb_flags & CTRL)) {
			if(kb_flags & (CAPS_LOCK | LSHIFT | RSHIFT)) {
				char last_k = kbdusShift[scancode];
				kb_buf[last++] = last_k;
			} else {
				char last_k = kbdus[scancode];
				kb_buf[last++] = last_k;
			}

			scan_key_hooks(kb_buf[last - 1]);
			read_kb_buf(kb_buf, last);
		}

		if(last >= KB_BUF_SIZE) last = 0;
	}

	UNUSED(regs);
}

void init_keyboard() {
	register_interrupt_handler(IRQ1, keyboard_handler);
}