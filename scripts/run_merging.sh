#! /usr/bin/env bash

set -x

bin=tmp
src=~/crawler/3kk-5kk

time ls $src/*.bin | $bin/run_merger $src/merged.bin
