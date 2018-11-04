#include <kernel/screen.h>
#include <kernel/keyboard.h>
#include <kernel/isr.h>
#include <kernel/ports.h>
#include <kernel/common.h>
#include <string.h>
#include <kernel/vesa_graphics.h>
#include <kernel/kernel.h>
#include <kernel/logger.h>

#define BACKSPACE 0x0E
#define ENTER 0x1C

static char key_buffer[256];
extern uint64_t GFX_MODE;

#define SC_MAX 57

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
		'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void keyboard_callback(registers_t *regs) {
	uint8_t scancode = port_byte_in(0x60);
	
	if(scancode > SC_MAX) return;
	if(scancode == BACKSPACE) {
		key_buffer[strlen(key_buffer) - 1] = '\0'; // backspace
		kprint_backspace();
	} else if(scancode == ENTER && !VBE_HD) {
		kprint("\n");
		user_input(key_buffer);
		key_buffer[0] = '\0';
	} else {
		char letter = sc_ascii[(int) scancode];
		char str[2] = { letter, '\0' };
		key_buffer[strlen(key_buffer)] = letter;
		key_buffer[strlen(key_buffer) + 1] = '\0';
		kprint(str);
	}
	
	UNUSED(regs);
}

void init_keyboard() {
	register_interrupt_handler(IRQ1, keyboard_callback);
}