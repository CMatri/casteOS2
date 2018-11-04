C_SOURCES = $(wildcard libc/*.c kernel/boot/*.c kernel/cpu/*.c kernel/devices/*.c kernel/io/*.c kernel/mem/*.c kernel/misc/*.c kernel/sys/*.c kernel/*.c)
NASM_SOURCES = $(wildcard kernel/boot/*.asm kernel/cpu/*.asm kernel/devices/*.asm kernel/io/*.asm kernel/mem/*.asm kernel/misc/*.asm kernel/sys/*.asm kernel/*.asm)
HEADERS = $(wildcard include/libc/*.h include/kernel/boot/*.h include/kernel/cpu/*.h include/kernel/devices/*.h include/kernel/io/*.h include/kernel/mem/*.h include/kernel/misc/*.h include/kernel/sys/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o} # cpu/interrupt.o cpu/gdt_init.o cpu/paging_init.o } 
OBJNASM = ${NASM_SOURCES:.asm=.o}

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32 -I./include

kernel.bin: ${OBJNASM} ${OBJ}
	i386-elf-gcc -o $@ -T linker.ld $^ -ffreestanding -O2 -nostdlib -lgcc
	cp kernel.bin isodir/boot/myos.bin

kernel.elf: ${OBJNASM} ${OBJ}
	i386-elf-gcc -o $@ -T linker.ld $^ -ffreestanding -O2 -nostdlib -lgcc

myos.iso: kernel.bin
	grub-mkrescue -o myos.iso isodir

run: myos.iso
	qemu-system-i386 -cdrom myos.iso -serial file:serial.log -m 2G	
	echo "Log: \n" && cat serial.log
	
runerr: myos.iso
	qemu-system-i386 -cdrom myos.iso -d guest_errors,int -serial file:serial.log -m 2G
	echo "Log: \n" && cat serial.log

runbin: kernel.bin
	qemu-system-i386 -kernel kernel.bin -serial file:serial.log -m 2G

# Open the connection to qemu and load our kernel-object file with symbols
debug: kernel.bin kernel.elf myos.iso
	qemu-system-i386 -s -cdrom myos.iso -d guest_errors,int -serial file:serial.log -m 2G &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file /home/matri/Programming/OSgrub/kernel.elf" -ex "set print pretty on" -ex "hb bpointdebug"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.iso
	rm -rf kernel/*/*.o kernel/*.o libc/*.o boot/grub/*.bin