#!/usr/bin/env bash

set -ex

function download {
    mkdir -p ${year}
    file=${year}/${day}.input.txt
    if [ ! -f ${file} ]; then
        curl -o ${file} "https://adventofcode.com/${year}/day/${day}/input" \
            -H 'Cookie: session=53616c7465645f5f7469ee8c36d4ee0e533cd0ba44481af9c413d0830ae357efd2fb913598694714002ebd7518dd470107c8d541ca05c70bf4c06d95e076aab8'
    fi
}

function download_today {
    year=$(TZ=EST date '+%Y')
    day=$(TZ=EST date '+%-d')
    download
}

function download_old {
    for year in $(seq 2015 2024); do
        for day in $(seq 1 25); do
            download
        done
    done
    year=2024
    for day in $(seq 1 7); do
        download
    done
}

download_old
