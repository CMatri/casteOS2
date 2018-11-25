extern main
extern init_stdio

section .text
	; push argv argc here
	call main 
	jmp $