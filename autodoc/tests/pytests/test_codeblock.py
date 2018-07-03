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
        assert len(doc.children) == 1

        block = doc.children[0]
        assert isinstance(block, nodes.literal_block)
        assert len(block.children) == 1
        assert block.rawsource == "bla bla\n  line 2"

        assert 'lang' not in block.attributes
        assert 'filename' not in block.attributes

        node = block.children[0]
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
        assert len(doc.children) == 1

        block = doc.children[0]
        assert isinstance(block, nodes.literal_block)
        assert len(block.children) == 1
        assert block.rawsource == "class text:\n    pass"

        assert 'lang' not in block.attributes
        assert 'filename' not in block.attributes

        node = block.children[0]
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
        assert len(doc.children) == 1

        block = doc.children[0]
        assert isinstance(block, nodes.literal_block)
        assert len(block.children) == 1
        assert block.rawsource == "class text:\n    pass"

        assert block.attributes.get('lang') == 'py'
        assert 'filename' not in block.attributes

        node = block.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == "class text:\n    pass"
