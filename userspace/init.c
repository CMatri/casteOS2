#include <stdio.h>
#include <sys/syscall.h>

void main() {
	while(1) {
		printf("\n>");

		char input_string[20];
		getstr(input_string);

		int i = -1;//findCommand(input_string);

		if(i >= 0)
		{
	//	    void (*command_function)(void);
	//		command_function = commands[i].func;
	//		(*command_function)();
	//		history[commands_history_index++] = commands[i];

	//		commands_history_operating_index++;
		}
		else 
		{
			printf("Command not found...\n");
		}
	}
}