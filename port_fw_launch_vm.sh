sudo qemu-system-x86_64 -hda alpine2600.img -nographic -device e1000,netdev=net0 -netdev user,id=net0,hostfwd=tcp::2222-:2222
