#!/bin/sh
set -e
. ./iso.sh
nm /home/matri/Programming/OSgrub/kernel/kernel.elf | grep " T " | awk '{ print "0x"$1" "$3 }' > /home/matri/Programming/OSgrub/kernel/kernel.sym
bochs