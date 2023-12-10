#!/bin/sh

# The password will also be used by the root user
USERNAME="renji"
PASSWORD="2600"

# Install minimal utilities, a C compiler and libc
apk add openrc neovim util-linux build-base nasm bash make shadow openssh strace

# Setup serial terminal access for QEMU
ln -s agetty /etc/init.d/agetty.ttyS0
echo ttyS0 > /etc/securetty

rc-update add agetty.ttyS0 default
rc-update add root default

## Creating our standard user
useradd -m $USERNAME
echo "$USERNAME:$PASSWORD" | chpasswd

## YES, THE PASSWORD IS IN CLEAR TEXT BUT WE DON'T CARE
echo "root:$PASSWORD" | chpasswd

## A few convenient changes
echo "alpine-2600" > /etc/hostname

### Mount pseudo fs
rc-update add devfs boot
rc-update add procfs boot
rc-update add sysfs boot

## Compiling all the tests and putting them in our user's folder
TEST_PATH=$MODULE_PATH/tests
TEST_PATH=`realpath $TEST_PATH`
for test in `find $TEST_PATH -mindepth 1 -maxdepth 1`; do
        echo "[alpine_rootfs_setup] Compiling test program $test"
        cd $test
        make >/dev/null 2>&1
        cp "$test/`basename $test`" /home/$USERNAME
done

chown -R $USERNAME:$USERNAME /home/$USERNAME

### add network 
echo -e "auto eth0\niface eth0 inet dhcp" | tee /etc/network/interfaces
rc-update add networking boot

### add openssh 
sed -i -e 's/#Port 22/Port 2222/' -e 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
rc-update add sshd boot

### Copy directories into the mounted tmpfs
for d in bin etc lib root home sbin usr; do tar c "/$d" | tar x -C /my-rootfs; done

### Create special dirs
for dir in dev proc run sys var; do mkdir /my-rootfs/${dir}; done

### Exit docker
exit
