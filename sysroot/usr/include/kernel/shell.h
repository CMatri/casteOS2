#ifndef SHELL_H
#define SHELL_H

#define MAX_COMMANDS 100
#define MAX_COMMANDS_HISTORY 100

#include <stdint.h>

typedef struct {
	char *name;
	char *description;
	void *func;
} command_table_t;

extern void shell();
extern void add_new_command();
extern void help_command();
extern void hello_command();
extern void reboot_command();
extern void clear_command();
extern void empty_command();

#endif