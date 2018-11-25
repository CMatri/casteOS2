CFLAGS:=-m32 -nostdlib -lc -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs --sysroot=../sysroot -isystem=/usr/include
LDFLAGS:= -T linker.ld
OBJS:=\
init.o \

userprogs: $(OBJS)
	nasm -felf32 start.asm -o start.o
	i386-elf-gcc start.o $< -o $<.cmod $(CFLAGS) -Wl,$(LDFLAGS)

install: userprogs
	cp *.cmod ../isodir/modules

clean:
	rm *.o *.cmod

%.o: %.c
	i386-elf-gcc -c $< -o $@ $(CFLAGS)

%.o: %.asm
	nasm -felf32 $< -o $@