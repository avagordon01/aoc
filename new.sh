#!/usr/bin/env bash

set -ex

year=$(TZ=EST date '+%Y')
day=$(TZ=EST date '+%-d')
cp starter.cc ${year}/${day}.cc
code -g ${year}/${day}.cc
firefox https://adventofcode.com/${year}/day/${day}

./download.sh

touch CMakeLists.txt
./build.sh
