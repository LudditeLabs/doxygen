import pytest
from textwrap import dedent
from docutils import nodes
import visitor


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
        assert len(doc.children) == 1

        node = doc.children[0]
        assert isinstance(node, nodes.paragraph)
        assert len(node.children) == 1

        node = node.children[0]
        assert isinstance(node, nodes.Text)
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
        assert isinstance(node, nodes.paragraph)

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == ('Lorem ipsum dolor sit amet,'
                             '\nconsectetur adipiscing elit,')

        node = doc.children[1]
        assert isinstance(node, nodes.paragraph)

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == ('sed do eiusmod tempor incididunt ut labore'
                             '\net dolore magna aliqua.')

    # Test: <div> tag is not supported by docutils.
    # It represented as paragraph with attr div=1.
    def test_div(self):
        doc = visitor.parse(dedent("""
        <div>Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        </div>
        """))

        assert len(doc.children) == 1

        node = doc.children[0]
        assert isinstance(node, nodes.paragraph)
        assert node.attributes.get('div') == '1'

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == ('Lorem ipsum dolor sit amet,\n'
                             'consectetur adipiscing elit.')

    # Test: <pre> tag is not supported by docutils.
    # It represented as paragraph with attr pre=1.
    def test_pre(self):
        doc = visitor.parse(dedent("""
        <pre>Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        </pre>
        """))

        assert len(doc.children) == 1

        node = doc.children[0]
        assert isinstance(node, nodes.paragraph)
        assert node.attributes.get('pre') == '1'

        node = node.children[0]
        assert isinstance(node, nodes.Text)
        assert len(node.children) == 0
        assert str(node) == ('Lorem ipsum dolor sit amet,\n'
        'consectetur adipiscing elit.')
