#!/usr/bin/env bash

set -ex

./build.sh

./download.sh

year=$(TZ=EST date '+%Y')
day=$(TZ=EST date '+%-d')
./install/bin/${year}-${day}
