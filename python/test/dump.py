#!/usr/bin/python

import pykyaml as kyaml
import sys

def dump(input, output):
    parser = kyaml.parser(input)
    root = parser.parse()

    output.write('%s\n' % root)

if __name__ == '__main__':
    dump(sys.stdin, sys.stdout)
