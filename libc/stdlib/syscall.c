#include <sys/syscall.h>

long syscall(int id, int ebx, int ecx, int edx) {
	int rv;
	asm volatile("int $0x70" : "=a"(rv) : "a"(id), "b"(ebx), "c"(ecx), "d"(edx));
	return rv;
}