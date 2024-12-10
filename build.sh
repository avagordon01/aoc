#!/usr/bin/env bash

set -ex

#Ninja Multi-Config allows quick & easy switching between Debug and Release builds without removing and rebuilding the whole build dir
if [ ! -d build ]; then
    mkdir -p build
    cmake -G "Ninja Multi-Config" -S . -B build || rm -rf build
fi
if [ -d build ]; then
    config=Release
    cmake --build build --config ${config}
    cmake --install build --config ${config} --prefix install
fi
