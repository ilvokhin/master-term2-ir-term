#! /usr/bin/env bash

set -x

bin=tmp
threads=8

src=/mnt/BAZA/raw_data2

for part in /mnt/BAZA/raw_data2/*; do
  time ls $part/*.gz | xargs -I{} -P$threads bash -c "zcat {} | $bin/run_indexer {}.bin"
done
