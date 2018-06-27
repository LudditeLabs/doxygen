import pytest
from textwrap import dedent
from docutils import nodes
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

        ## subscript/superscript
        # html
        (nodes.subscript, r'<sub>multiple words</sub>', 'multiple words'),
        (nodes.superscript, r'<sup>multiple words</sup>', 'multiple words'),

        (nodes.inline, r'<center>multiple words</center>', 'multiple words'),
        (nodes.inline, r'<small>multiple words</small>', 'multiple words'),
        (nodes.inline, r'<span>multiple words</span>', 'multiple words'),
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

    # Test: node attributes.
    @pytest.mark.parametrize('cls,docstring,attrs', [
        (nodes.emphasis, r'\a word', {}),

        # All tag attributes are stored in 'extra' field.
        (nodes.emphasis, '<em myattr=1 val="str">word</em>', {
            'extra': {'myattr':'1', 'val':"str"}
        }),
        (nodes.strong, r'<b>multiple words</b>', {}),
        (nodes.strong, r'<b c="">multiple words</b>', {'extra': {'c': ''}}),

        # Tags not supported by docutils has docutils node with special attr.
        (nodes.inline, r'<center>words</center>', dict(centered='1')),
        (nodes.inline, r'<small>words</small>', dict(small='1')),
        (nodes.inline, r'<span>words</span>', dict(span='1')),
    ])
    def test_attrs(self, cls, docstring, attrs):
        doc = visitor.parse(docstring)
        assert isinstance(doc, nodes.document)

        para = doc.children[0]
        assert isinstance(para, nodes.paragraph)
        assert len(para.children) == 1

        node = para.children[0]
        assert isinstance(node, cls)

        d = {k: v for k, v in node.attributes.items() if k in attrs}
        assert d == attrs

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

    # TODO: move to test_reference.py

    # Test: url in text.
    def test_url(self):
        doc = visitor.parse('some text http://example.com')
        assert isinstance(doc, nodes.document)

        para = doc.children[0]
        assert isinstance(para, nodes.paragraph)
        assert len(para.children) == 2

        node = para.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == 'some text '

        node = para.children[1]
        assert isinstance(node, nodes.reference)
        assert len(node.children) == 1
        assert node.get('refuri') == 'http://example.com'

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == 'http://example.com'

    # Test: mailto in text.
    def test_mailto(self):
        doc = visitor.parse('some text example@bla.com')
        assert isinstance(doc, nodes.document)

        para = doc.children[0]
        assert isinstance(para, nodes.paragraph)
        assert len(para.children) == 2

        node = para.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == 'some text '

        node = para.children[1]
        assert isinstance(node, nodes.reference)
        assert len(node.children) == 1
        assert node.get('refuri') == 'mailto:example@bla.com'

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == 'example@bla.com'
