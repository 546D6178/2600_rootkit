#!/bin/sh

INITRAMFS_BUILD=$(pwd)/initramfs
BUSYBOX_BUILD=$(pwd)/busybox
BUILDS=$(pwd)/output

## Creating initramfs folder and subfolders
mkdir $INITRAMFS_BUILD
mkdir -p $INITRAMFS_BUILD/bin $INITRAMFS_BUILD/sbin $INITRAMFS_BUILD/etc $INITRAMFS_BUILD/proc $INITRAMFS_BUILD/sys $INITRAMFS_BUILD/dev $INITRAMFS_BUILD/usr/bin $INITRAMFS_BUILD/usr/sbin
## Copying every busybox executable into our initramfs root
cp -a $BUSYBOX_BUILD/_install/* $INITRAMFS_BUILD/.

## Creating dynamic libraries folder
mkdir $INITRAMFS_BUILD/lib
mkdir $INITRAMFS_BUILD/lib64

## Compiling init_loop executable
gcc src/my_init_loop.c -o$INITRAMFS_BUILD/init_loop

## Copying the libraries from our main system to our initramfs
cp /lib/libm.so.6 $INITRAMFS_BUILD/lib
cp /lib/libc.so.6 $INITRAMFS_BUILD/lib
cp /lib/libresolv.so.2 $INITRAMFS_BUILD/lib
cp /lib/ld-linux-x86-64.so.2 $INITRAMFS_BUILD/lib

cp /lib64/libm.so.6 $INITRAMFS_BUILD/lib64
cp /lib64/libc.so.6 $INITRAMFS_BUILD/lib64
cp /lib64/libresolv.so.2 $INITRAMFS_BUILD/lib64
cp /lib64/ld-linux-x86-64.so.2 $INITRAMFS_BUILD/lib64

cp -r $INITRAMFS_BUILD/lib $INITRAMFS_BUILD/usr/lib
cp -r $INITRAMFS_BUILD/lib64 $INITRAMFS_BUILD/usr/lib64

## Copying the init script for our initramfs
cp src/init $INITRAMFS_BUILD/init

## Setting the init script as executable
chmod +x $INITRAMFS_BUILD/init

## Creating the initramfs image
cd $INITRAMFS_BUILD
find . -print0 | cpio --null -ov --format=newc | gzip -9 > $BUILDS/initramfs.cpio.gz
