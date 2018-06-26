import pytest
import sys

def dump(node, indent=0):
    offset = ' ' * indent
    sys.stderr.write('{}{}'.format(offset, node.shortrepr()))
    # print(offset, node.shortrepr())
    for c in node.children:
        dump(c, indent + 2)


def pytest_namespace():
    return {'g': {
        'dump': dump,
    }}
