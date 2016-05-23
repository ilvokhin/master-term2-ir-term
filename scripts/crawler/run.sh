#! /usr/bin/env bash

set -x

threads=32
chunk=32768
pool=pool

rm -rf $pool
mkdir $pool

split -l $chunk -a 5 id.txt pool/id.txt.

ls $pool/id.txt.* | xargs -I{} -P$threads bash -c "cat {} | python vk_crawler.py > {}.out 2> {}.log"
