global user_mode_function

user_mode_function:
	int 0x80
	int 0x80
	ret