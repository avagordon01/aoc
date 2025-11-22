#!/usr/bin/env bash

set -ex

#Ninja Multi-Config allows quick & easy switching between Debug and Release builds without removing and rebuilding the whole build dir
cmake -G "Ninja Multi-Config" -S . -B build
config=Debug
cmake --build build --config ${config} --parallel 1
cmake --install build --config ${config} --prefix install
