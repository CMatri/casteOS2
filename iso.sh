#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/modules
cp userspace/*.cmod isodir/modules

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=1
set default=0

menuentry "Boot casteOS" {
	multiboot /boot/myos.kernel
	module /modules/program.cmod MYPROGRAM
	module /modules/program.cmod MYPROGRAM2
}
EOF
grub-mkrescue -d /usr/lib/grub/i386-pc/ -o myos.iso isodir/

