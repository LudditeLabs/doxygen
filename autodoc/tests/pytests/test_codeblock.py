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
from textwrap import dedent
from docutils import nodes
import visitor


# Test: code blocks.
class TestCodeBlock:
    def test_verbatim(self):
        doc = visitor.parse(dedent("""
        @verbatim
        bla bla
          line 2
        @endverbatim
        """))
        assert isinstance(doc, nodes.document)
        assert len(doc) == 1

        block = doc[0]
        assert isinstance(block, nodes.literal_block)
        assert len(block) == 1
        assert block.rawsource == "bla bla\n  line 2"

        assert 'lang' not in block
        assert 'filename' not in block

        node = block[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == "bla bla\n  line 2"

    def test_code_nolang(self):
        doc = visitor.parse(dedent(r"""
        \code
        class text:
            pass
        \endcode
        """))
        assert isinstance(doc, nodes.document)
        assert len(doc) == 1

        block = doc[0]
        assert isinstance(block, nodes.literal_block)
        assert len(block) == 1
        assert block.rawsource == "class text:\n    pass"

        assert 'lang' not in block
        assert 'filename' not in block

        node = block[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == "class text:\n    pass"

    def test_code_withlang(self):
        doc = visitor.parse(dedent(r"""
        \code{.py}
        class text:
            pass
        \endcode
        """))
        assert isinstance(doc, nodes.document)
        assert len(doc) == 1

        block = doc[0]
        assert isinstance(block, nodes.literal_block)
        assert len(block) == 1
        assert block.rawsource == "class text:\n    pass"

        assert block.get('lang') == 'py'
        assert 'filename' not in block

        node = block[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == "class text:\n    pass"
