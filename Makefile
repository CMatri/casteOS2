C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o cpu/gdt_init.o cpu/paging_init.o } 

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

kernel.bin: boot/grub_boot.o ${OBJ}
	i386-elf-gcc -o $@ -T linker.ld $^ -ffreestanding -O2 -nostdlib -lgcc
	cp kernel.bin isodir/boot/myos.bin

kernel.elf: boot/grub_boot.o ${OBJ}
	i386-elf-gcc -o $@ -T linker.ld $^ -ffreestanding -O2 -nostdlib -lgcc

myos.iso: kernel.bin
	grub-mkrescue -o myos.iso isodir

run: myos.iso
	qemu-system-i386 -cdrom myos.iso -serial file:serial.log -m 4G
	echo "Log: \n" && cat serial.log

runbin: kernel.bin
	qemu-system-i386 -kernel kernel.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: kernel.bin kernel.elf
	qemu-system-i386 -s -cdrom myos.iso -d guest_errors,int #&
	#${GDB} -ex "target remote localhost:1234" -ex "symbol-file /home/matri/Programming/OSgrub/kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o