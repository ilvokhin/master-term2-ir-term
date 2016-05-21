#! /usr/bin/env bash

set -x

bin=tmp
threads=8


src=~/crawler/3kk-5kk

time ls $src/*out | xargs -I{} -P$threads bash -c "cat {} | $bin/run_indexer > {}.bin"
