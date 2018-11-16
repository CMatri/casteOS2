#include <kernel/shell.h>
#include <stdio.h>

command_table_t commands[MAX_COMMANDS];
command_table_t history[MAX_COMMANDS_HISTORY];

int num_of_commands;
int commands_history_index;
int commands_history_operating_index = -1;

void shell() {
	while(1) {
		printf("\n>");

		char input_string[20];
		getstr(input_string);

		int i = -1;//findCommand(input_string);

		if(i >= 0)
		{
		    void (*command_function)(void);
			command_function = commands[i].func;
			(*command_function)();
			history[commands_history_index++] = commands[i];

			commands_history_operating_index++;
		}
		else 
		{
			printf("Command not found...\n");
		}
	}
}

void add_new_command() {}
void help_command() {}
void hello_command() {}
void reboot_command() {}
void clear_command() {}
void empty_command() {}