GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb

run:
	qemu.sh

clean:
	clean.sh

debug:
	qemu-system-i386 -s -cdrom myos.iso -boot d -serial file:serial.log -m 2G -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel/kernel.elf" -ex "hb bpointdebug"

bochs:
	bochs.sh

all:
	run