from docutils import nodes
from textwrap import dedent
import pytest
import visitor

# TODO: test html commands.
# TODO: test xml commands.

# Test: inline markup commands.
class TestInline:
    # Test: simple commands.
    @pytest.mark.parametrize('cls,docstring,text', [
        ## italics
        # doxygen
        (nodes.emphasis, r'\a word', 'word'),
        (nodes.emphasis, r'\e word', 'word'),
        (nodes.emphasis, r'@em word', 'word'),
        # html
        (nodes.emphasis, '<em>multiple words</em>', 'multiple words'),
        (nodes.emphasis, '<em a=2>multiple words</em>', 'multiple words'),

        ## bold
        # doxygen
        (nodes.strong, r'\b word', 'word'),
        # html
        (nodes.strong, r'<strong>word</strong>', 'word'),
        (nodes.strong, r'<b>multiple words</b>', 'multiple words'),

        ## typewriter/code
        # doxygen
        (nodes.literal, r'\c word', 'word'),
        (nodes.literal, r'\p word', 'word'),
        # html
        (nodes.literal, r'<tt>multiple words</tt>', 'multiple words'),
        # xml
        (nodes.literal, r'<c>multiple words</c>', 'multiple words'),


    ])
    def test_tag(self, cls, docstring, text):
        doc = visitor.parse(docstring)
        assert isinstance(doc, nodes.document)

        para = doc.children[0]
        assert isinstance(para, nodes.paragraph)
        assert len(para.children) == 1

        node = para.children[0]
        assert isinstance(node, cls)
        assert len(node.children) == 1

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == text

    # Test: all tag attributes are stored in 'extra' field.
    @pytest.mark.parametrize('cls,docstring,attrs', [
        (nodes.emphasis, r'\a word', None),
        (nodes.emphasis, '<em myattr=1 val="str">word</em>', dict(myattr='1', val="str")),
        (nodes.strong, r'<b>multiple words</b>', None),
        (nodes.strong, r'<b c="">multiple words</b>', dict(c='')),
    ])
    def test_attr(self, cls, docstring, attrs):
        doc = visitor.parse(docstring)
        assert isinstance(doc, nodes.document)

        para = doc.children[0]
        assert isinstance(para, nodes.paragraph)
        assert len(para.children) == 1

        node = para.children[0]
        assert isinstance(node, cls)
        assert node.attributes.get('extra') == attrs


    # Test: (non-standard construction) multiple words, nested tag.
    def test_multi_nested(self):
        doc = visitor.parse(r"<em>multiple \a words</em>")

        assert isinstance(doc, nodes.document)

        para = doc.children[0]
        assert isinstance(para, nodes.paragraph)
        assert len(para.children) == 1

        node = para.children[0]
        assert isinstance(node, nodes.emphasis)
        assert len(node.children) == 2

        emp = node.children[1]
        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        # TODO: trailing space, improve me.
        assert str(node) == 'multiple '

        assert isinstance(emp, nodes.emphasis)
        assert len(emp.children) == 1

        node = emp.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == 'words'
