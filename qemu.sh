#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST ) -cdrom myos.iso -boot d -hda disk.img -serial file:serial.log -m 2G
echo "Log: \n" && cat serial.log
#qemu-system-x86_64 -cdrom myos.iso -boot d -hda c.img -m 250M