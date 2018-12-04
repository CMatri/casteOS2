#include <stdio.h>
#include <sys/syscall.h>

#if defined(__is_myos_kernel)
#include <kernel/screen.h>
#endif

int putchar(int ic)
{
#if defined(__is_myos_kernel)
	char c = (char) ic;
	kprint_char(c);
#else
	syscall(4, ic, 0, 0);
#endif
	return ic;
}
