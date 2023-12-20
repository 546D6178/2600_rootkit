# 2600 Rootkit Group 14

## Introduction
This is a rootkit we have to make as a school project. Nothing more

## Prerequisites
- Docker
- kpartx
- GNU utils (such as gcc, make, etc)
- parted
- QEMU
- GRUB
- Root permissions
- A functional brain

## Steps
1. Clone this repository 
`$ git clone github.com/Salim-Dardouchi/2600_rootkit`
2. Change directory 
`$ cd 2600_rootkit`
3. Copy Linux sources (Preferably the last Linux 5 LTS)
`$ cp -r <linux_sources_location> ./linux`
4. Move into the Linux directory, configure and compile the kernel
`$ cd ./linux && make defconfig && make -j <threads>`
5. You're ready to work

## Usage

### "make_modules.sh"
This one is straightforward, it compiles every module in the modules folder

### "create_diskfs.sh"
- 1st arg: size (ex: `1024M`)
- 2nd arg: img path (ex: `./2600.img`)
- 3rd arg: tempfolder path (ex: `/tmp/rootfs`)
- 4th arg: module path to copy in diskfs (ex: `./modules/rootkit`)

### QEMU
Once your image is ready, you can use QEMU to make a virtual machine:
`sudo qemu-system-x86_64 -hda <image_file> -nographic`
(you need superuser permissions because the image file is readable by root only although you can chown it)

### Exploitation
You have now to load the rootkit : `insmod /module/malicious_file_rootkit/rootkit.ko`  
Clients are here : `/module/clients`

