#! /usr/bin/env bash

set -x

bin=tmp
#src=/mnt/BAZA/raw_data2/*/*bin

time ls /mnt/BAZA/raw_data2/*/*bin | $bin/run_merger /mnt/BAZA/raw_data2/merged.bin
