global switch_to_user_mode
global switch_kernel_task

switch_to_user_mode:
	mov ebx,[esp+8]
	mov ecx,[esp+4]
	mov ax,0x23
	mov ds,ax
	mov es,ax 
	mov fs,ax 
	mov gs,ax ;we don't need to worry about SS. it's handled by iret

	mov eax,esp
	push 0x23 ;user data segment with bottom 2 bits set for ring 3
	push ebx ;push given stack
	pushf
	push 0x1B; ;user code segment with bottom 2 bits set for ring 3
	push ecx 
	iret
	pop eax
	pop eax
	ret	

switch_kernel_task:
	mov ebx,[esp+8]
	mov ecx,[esp+4]
	mov ax,0x10
	mov ds,ax
	mov es,ax 
	mov fs,ax 
	mov gs,ax ;we don't need to worry about SS. it's handled by iret

	mov eax,esp
	push 0x10 ;user data segment with bottom 2 bits set for ring 3
	push ebx ; push given stack BECAUSE IMPORTANT BITCH
	pushf
	push 0x08; ;user code segment with bottom 2 bits set for ring 3
	push ecx 
	iret
	pop eax
	pop eax
	ret	