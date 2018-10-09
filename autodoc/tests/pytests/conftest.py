# Copyright (C) 2018, Luddite Labs Inc.
#
# This file is part of doxygen.
#
# Doxygen is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
