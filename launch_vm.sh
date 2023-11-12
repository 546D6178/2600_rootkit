#!/bin/sh

qemu-system-x86_64 -kernel linux/arch/x86/boot/bzImage -m 512 -initrd output/initramfs.cpio.gz -append "console=ttyS0" -nographic -s
