#!/bin/sh

cd linux
LINUX_PATH="$(pwd)"
cd ..
for dir in $(find ./modules -mindepth 1 -maxdepth 1 -type d); do
    echo "Building module $(echo $dir | cut -d'/' -f3)";
    cd $dir
    make -C $LINUX_PATH M="$(pwd)" modules;
    cd ../..
done

