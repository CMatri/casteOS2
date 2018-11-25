#!/bin/sh
set -e

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/modules

. ./build.sh

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=1
set default=0

menuentry "Boot casteOS" {
	multiboot /boot/myos.kernel
	module /modules/init.o.cmod init_user
	module /modules/initrd.img initrd
#	module /modules/kickitwconnor.bmp kickitwconnor
}
EOF
grub-mkrescue -d /usr/lib/grub/i386-pc/ -o myos.iso isodir/

