#!/bin/sh

IMAGE_PATH="$(pwd)/alpine2600.img"

if [[ "$1" ]]; then
	IMAGE_PATH=$1
fi

echo "Launching QEMU with image: $IMAGE_PATH"
sleep 2

qemu-system-x86_64 -nographic $IMAGE_PATH
