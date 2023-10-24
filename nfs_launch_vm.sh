#!/bin/sh

qemu-system-x86_64 \
 -kernel linux-5.15.131/arch/x86/boot/bzImage \
 -m 1G \
 -enable-kvm \
 -smp 6 \
 -append " \
 	init=/init \
	console=ttyS0 \
	root=/dev/nfs \
	nfsroot=10.0.2.2:/srv/nfs,vers=3 \
 	rw \
	ip=10.0.2.15::10.0.2.1:255.255.255.0\
	raid=noautodetect" \
  -nographic \
  -s
