#!/usr/bin/env bash

set -ex

year=$(date '+%Y')
day=$(date '+%-d')
file=${day}.input.txt
if [ ! -f ${file} ]; then
    curl -o ${file} "https://adventofcode.com/${year}/day/${day}/input" \
        -H 'Cookie: session=53616c7465645f5f7469ee8c36d4ee0e533cd0ba44481af9c413d0830ae357efd2fb913598694714002ebd7518dd470107c8d541ca05c70bf4c06d95e076aab8'
fi
