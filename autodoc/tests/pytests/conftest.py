import pytest
import sys
from docutils import nodes


def dump(node, indent=0):
    offset = ' ' * indent
    sys.stderr.write('{}{}\n'.format(offset, node.shortrepr()))
    for c in node.children:
        dump(c, indent + 2)


def assert_simple_par(node, text):
    __tracebackhide__ = True

    assert isinstance(node, nodes.paragraph)
    assert len(node) == 1

    node = node[0]
    assert isinstance(node, nodes.Text)
    assert len(node.children) == 0
    assert str(node) == text


def pytest_namespace():
    return {'g': {
        'dump': dump,
        'assert_simple_par': assert_simple_par,
    }}
