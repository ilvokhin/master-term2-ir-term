#! /usr/bin/env python
# -*- coding: utf-8 -*

import sys
from math import log

def calc_cg(marks):
    return sum(marks)


def calc_dcg(marks):
    out = marks[0]
    for pos, elem in enumerate(marks):
        if pos == 0:
            continue
        out += elem / log(pos + 1, 2)
    return out


def calc_ndcg(marks):
    return (calc_dcg(marks) / calc_dcg(sorted(marks, reverse=True)))


def main():
    stats = []
    for line in sys.stdin:
        lst = line.strip().split('\t')
        marks = lst[1::]
        stats.append(map(float, marks))

    cg = sum([calc_cg(marks) for marks in stats])
    dcg = sum([calc_dcg(marks) for marks in stats])
    ndcg = sum([calc_ndcg(marks) for marks in stats])

    print 'cg', cg
    print 'dcg', dcg
    print 'ndcg', ndcg

if __name__ == "__main__":
  main()
