#!/bin/bash

# ./build_lib.sh <name> <src> <path>

echo "Building: $1"

pushd $3

if [ ! -d "build" ]; then
    mkdir build
    pushd build
    cmake ..
    popd
fi

pushd build

cmake ..

make

make install

popd

popd
