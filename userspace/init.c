#include <stdio.h>

void main() {
	printf(">");
	char c = getch();
	printf("Hello from user mode program: %c", c);
}