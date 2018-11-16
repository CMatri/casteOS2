#include <stdio.h>

#if defined(__is_myos_kernel)
#include <kernel/screen.h>
#endif

int putchar(int ic)
{
#if defined(__is_myos_kernel)
	char c = (char) ic;
	kprint_char(c);
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}
