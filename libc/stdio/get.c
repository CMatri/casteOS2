#include <stdio.h>
#if defined(__is_myos_kernel)
#include <kernel/keyboard.h>
#endif

int getchar()
{
	int c = getch();
	putchar(c);
	return c;
}

int getch()
{
#if defined(__is_myos_kernel)
	char c = 0;
	while(!(c = keyboard_get_key()));
	return c;
#else
	char c = 0;
	while(!(c = syscall(3, 0, 0, 0)));
	return c;
#endif
}

char* getstr(char *str)
{
	int c = getch();
	int i = 0;

	while(c != '\n')
	{
		if(c != '\b')
		{
			str[i++] = c;
			putchar(str[i-1]);
		} 
		else if (c == '\b' && i > 0)
		{
			str[--i] = 0;
			putchar(c);
		}

		c = getch();
	}

	str[i] = '\0';
	putchar('\n');
	return str;
}