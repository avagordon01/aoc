#!/usr/bin/env bash

set -ex

./build.sh

#./download.sh

#year=$(TZ=EST date '+%Y')
#day=$(TZ=EST date '+%-d')
#./install/bin/${year}-${day}

# ./install/bin/2023-12
./install/bin/2025-6
