#!/usr/bin/env python3

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

import sys
import os.path as op
import pickle
import sqlite3


def dump(node, indent=0):
    offset = ' ' * indent
    print(offset, node.shortrepr())
    for c in node.children:
        dump(c, indent + 2)


def dump_docblock(filename, rowid):
    if not op.exists(filename):
        sys.stderr.write("[ERROR] Can't find %s\n", filename)
        sys.exit(1)

    conn = sqlite3.connect(filename)
    res = conn.execute('SELECT doc FROM docblocks where rowid=?', (rowid,))
    res = res.fetchone()[0]

    if res is None:
        sys.stderr.write("[ERROR] Can't find row %d\n", rowid)
        sys.exit(1)

    document = pickle.loads(res)
    dump(document)

# if __name__ == '__main__':
#     document = pickle.load(open(sys.argv[1], 'rb'))
#     dump(document)


if __name__ == '__main__':
    rowid = int(sys.argv[1])
    if len(sys.argv) == 3:
        filename = sys.argv[2]
    else:
        filename = '/tmp/autodocdb/content.db'

    dump_docblock(filename, rowid)
