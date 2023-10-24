#!/bin/sh

# Install minimal utilities, a C compiler and libc
apk add openrc neovim util-linux build-base nasm bash shadow

# Setup serial terminal access for QEMU
ln -s agetty /etc/init.d/agetty.ttyS0
echo ttyS0 > /etc/securetty

rc-update add agetty.ttyS0 default
rc-update add root default

## YES, THE PASSWORD IS IN CLEAR TEXT BUT WE DON'T CARE
echo "root:2600" | chpasswd

## A few convenient changes
echo "alpine-2600" > /etc/hostname
mkdir /root 
chown -R root:root /root

### Mount pseudo fs
rc-update add devfs boot
rc-update add procfs boot
rc-update add sysfs boot

### Copy directories into the mounted tmpfs
for d in bin etc lib root sbin usr; do tar c "/$d" | tar x -C /my-rootfs; done

### Create special dirs
for dir in dev proc run sys var; do mkdir /my-rootfs/${dir}; done

### Exit docker
exit
