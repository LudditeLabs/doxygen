from docutils.nodes import document, paragraph, Text
import pytest
from textwrap import dedent
import visitor

# Test: parse empty docstring
def test_empty_document():
    doc = visitor.parse("")
    assert isinstance(doc, document)
    assert len(doc.children) == 0


class TestParagraph:
    # Test: single paragrapg.
    # NOTE: <same> is special value meaning use 'docstring' value.
    #       <lorem> is special value meaning use predefined 'Lorem ipsum' text.
    @pytest.mark.parametrize('docstring,text', [
        ('hello', '<same>'),
        ('Lorem ipsum dolor sit amet..', '<same>'),

        ("""
        Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        """, "<lorem>"),

        # html
        ("""
        <p>
        Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        </p>
        """, "<lorem>"),

        # xml
        ("""
        <para>
        Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        </para>
        """, "<lorem>"),

    ])
    def test_single(self, docstring, text):
        docstring = dedent(docstring)
        if text == '<same>':
            text = docstring
        elif text == '<lorem>':
            text = 'Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit.'

        doc = visitor.parse(docstring)
        assert isinstance(doc, document)
        assert len(doc.children) == 1

        node = doc.children[0]
        assert isinstance(node, paragraph)
        assert len(node.children) == 1

        node = node.children[0]
        assert isinstance(node, Text)
        assert len(node.children) == 0
        assert str(node) == text

    @pytest.mark.parametrize('docstring', [
        """
        Lorem ipsum dolor sit amet,
        consectetur adipiscing elit,

        sed do eiusmod tempor incididunt ut labore
        et dolore magna aliqua.
        """,

        """
        <p>Lorem ipsum dolor sit amet,
        consectetur adipiscing elit,</p>

        sed do eiusmod tempor incididunt ut labore
        et dolore magna aliqua.
        """,

        # NOTE: <br> is used to break line.
        """
        <p>Lorem ipsum dolor sit amet,<br>consectetur adipiscing elit,</p>

        <para>
        sed do eiusmod tempor incididunt ut labore
        et dolore magna aliqua.
        </para>
        """,

    ])
    def test_multi_paragraph(self, docstring):
        doc = visitor.parse(dedent(docstring))
        assert len(doc.children) == 2

        node = doc.children[0]
        assert isinstance(node, paragraph)

        node = node.children[0]
        assert isinstance(node, Text)
        assert len(node.children) == 0
        assert str(node) == ('Lorem ipsum dolor sit amet,'
                             '\nconsectetur adipiscing elit,')

        node = doc.children[1]
        assert isinstance(node, paragraph)

        node = node.children[0]
        assert isinstance(node, Text)
        assert len(node.children) == 0
        assert str(node) == ('sed do eiusmod tempor incididunt ut labore'
                             '\net dolore magna aliqua.')
