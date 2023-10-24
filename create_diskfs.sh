#!/bin/bash

## REQUIRES ROOT PERMISSIONS
if [ $(id -u) -ne 0 ]; then
    echo "This script requires root permissions"
    exit
fi

## This script basically creates a disk img using your kercompiled kernel and Alpine software

function cleanup() {
    ## Remove the setupscript script from the tmpfs
    echo "[create_diskfs] Removing the setup script from $TEMPFOLDER"
    rm $TEMPFOLDER/alpine_rootfs_setup.sh
    
    ## Unmount rootfs from host
    echo "[create_diskfs] Unmounting $TEMPFOLDER"
    umount $TEMPFOLDER
    
    ## Remove temp directory
    echo "[create_diskfs] Deleting $TEMPFOLDER"
    rm -rf $TEMPFOLDER
    
    ## Detach loop device
    echo "[create_diskfs] Detaching the $DISKFILE_NAME loop device"
    kpartx -d $DISKFILE
    losetup -D

    exit
}

## Trapping Ctrl+C and hiding ^C from term
stty -echoctl
trap 'cleanup' SIGINT 

if [ -z $1 ]; then
    SIZE=1024M
elif [ $1 = "-h" ]; then
    echo "Usage: $0 <size (ex: 1024M, 2G)> <diskfile path (ends in .img)> <tempfolder path> <module path if it exists>"
    exit
else
    SIZE=$1
fi

if [ -z $2 ]; then
    DISKFILE=$(pwd)/disk.img
else
    DISKFILE=$2
fi

if [ -z $3 ]; then
    TEMPFOLDER=/tmp/my-rootfs
else
    TEMPFOLDER=$3
fi


if [ -n $4 ]; then
    MODULE_PATH=$4
fi

echo -e "[create_diskfs] CONFIG:\n\tVirtual disk size: $SIZE\n\tDiskfile output: $DISKFILE\n\tTemporary Folder: $TEMPFOLDER\n\tModule path: $MODULE_PATH"

## Checking if diskfile already exists, if so, the script deletes it
if test -f "$DISKFILE"; then
    echo "[create_diskfs] $DISKFILE already exists, deleting it."
    rm -f $DISKFILE 
fi 

DISKFILE_NAME=$(echo $DISKFILE | grep -Po '(?!:\/)[a-zA-Z0-9_]*\.img$')

## Create a disk image that is bootable  
echo "[create_diskfs] Creating $DISKFILE_NAME and partitioning it"
truncate -s $SIZE $DISKFILE
/sbin/parted -s $DISKFILE mktable msdos
/sbin/parted -s $DISKFILE mkpart primary ext4 1 "100%"
/sbin/parted -s $DISKFILE set 1 boot on

## Mount it as a loopback device to create an EXT4 partition 
echo "[create_diskfs] Mounting $DISKFILE_NAME as a loopback device"

LOOPPART="/dev/mapper/$(kpartx -va $DISKFILE | cut -d' ' -f 3)"
echo "[create_diskfs] Loop partition - $LOOPPART"

## Format our ext4 partition and then mount it to our temp folder
echo "[create_diskfs] Formatting $DISKFILE_NAME's only partition as EXT4 and mounting it at $TEMPFOLDER"

mkfs.ext4 $LOOPPART
mkdir -p $TEMPFOLDER

mount $LOOPPART $TEMPFOLDER

## Copy our setup script to the rootfs folder
echo "[create_diskfs] Copying the Alpine setup script in $TEMPFOLDER"
cp src/alpine_rootfs_setup.sh $TEMPFOLDER

## Run an interactive Alpine Docker container that mounts the disk fs and setups it
echo "[create_diskfs] Running an Alpine container along with the setup script"
docker run --rm -v $TEMPFOLDER:/my-rootfs alpine /my-rootfs/alpine_rootfs_setup.sh

## Change welcome message
echo "[create_diskfs] Changing the distro's welcome message"
cat $TEMPFOLDER/etc/issue | sed s/Alpine/Alpine2600/g > $TEMPFOLDER/etc/issue

## Create the boot and boot/grub folders and then copy the Linux kernel on the tmpfs
echo "[create_diskfs] Copying the Linux kernel to $TEMPFOLDER/boot/vmlinuz"
mkdir -p $TEMPFOLDER/boot/grub
cp linux/arch/x86/boot/bzImage $TEMPFOLDER/boot/vmlinuz

## Copy the grub config on the tmpfs
echo "[create_diskfs] Copying the GRUB config to $TEMPFOLDER/boot/grub/grub.cfg"
cp src/grub.cfg $TEMPFOLDER/boot/grub/grub.cfg

## Copy the kernel module if it was specified
if [[ -n $MODULE_PATH ]]; then
    echo "[create_diskfs] Copying the kernel module at $MODULE_PATH to $TEMPFOLDER/root/"
    cp -r $MODULE_PATH $TEMPFOLDER/root/ 
fi

## Install grub on the tmpfs
LOOPDEVICE=$(losetup -l | head -n 2 | tail -n 1 | cut -d' ' -f 1)
echo "[create_diskfs] Installing GRUB on $LOOPDEVICE"
sudo grub-install --directory=/usr/lib/grub/i386-pc --boot-directory=$TEMPFOLDER/boot $LOOPDEVICE

## Cleanup
cleanup
