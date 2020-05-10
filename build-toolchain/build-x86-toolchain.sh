#!/bin/bash

docker build . \
    -f Dockerfile.toolchain \
    -t toolchain:i686-elf \
    --build-arg TARGET=i686-elf

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

rm -rf $DIR"/toolchain"

ctr=$(docker create toolchain:i686-elf)

docker cp $ctr:/toolchain/bin $DIR/toolchain

docker rm $ctr