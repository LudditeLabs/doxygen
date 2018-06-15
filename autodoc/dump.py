#!/usr/bin/env python3
import sys
import pickle


def dump(node, indent=0):
    offset = ' ' * indent
    print(offset, node.shortrepr())
    for c in node.children:
        dump(c, indent + 2)


if __name__ == '__main__':
    document = pickle.load(open(sys.argv[1], 'rb'))
    dump(document)
