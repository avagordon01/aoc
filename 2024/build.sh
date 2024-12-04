#!/usr/bin/env bash

set -ex

if [ ! -d build ]; then
    mkdir -p build/{debug,release}
    cmake -G Ninja -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug || rm -rf build
    cmake -G Ninja -S . -B build/release -DCMAKE_BUILD_TYPE=Release || rm -rf build
fi
if [ -d build ]; then
    #cmake --build build/debug
    cmake --build build/release
    cmake --install build/release --prefix install
fi
