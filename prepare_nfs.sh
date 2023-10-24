#!/bin/sh

mount --bind ./initramfs /srv/nfs
systemctl start nfs-server
