#! /usr/bin/env python
# -*- coding: utf-8 -*

import sys

P_E = 0.25

def load_marks(filename):
    out = []
    with open(filename) as f:
        for line in f:
            lst = line.strip().split('\t')
            out.append(map(float, lst[1::]))
    return out

def good_percent(exp_1, exp_2):
    return sum([elem_1 == elem_2 for elem_1, elem_2 in zip(exp_1, exp_2)])

def main():
    if len(sys.argv) < 3:
        sys.exit('WTF?')

    exp_1 = load_marks(sys.argv[1])
    exp_2 = load_marks(sys.argv[2])

    out = 0.0
    for a, b in zip(exp_1, exp_2):
        out += good_percent(a, b)

    print (out - P_E) / (1.0 - P_E)

if __name__ == "__main__":
  main()
