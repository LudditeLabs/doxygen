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
        assert len(doc) == 1
        pytest.g.assert_simple_par(doc[0], text)

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
        assert len(doc) == 2

        pytest.g.assert_simple_par(doc[0], 'Lorem ipsum dolor sit amet,'
                                           '\nconsectetur adipiscing elit,')

        pytest.g.assert_simple_par(doc[1],
                                   'sed do eiusmod tempor incididunt ut labore'
                                   '\net dolore magna aliqua.')

    # Test: <div> tag is not supported by docutils.
    # It represented as paragraph with attr div=1.
    def test_div(self):
        doc = visitor.parse(dedent("""
        <div>Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        </div>
        """))

        assert len(doc) == 1
        assert doc[0].get('div') == '1'
        pytest.g.assert_simple_par(doc[0], 'Lorem ipsum dolor sit amet,\n'
                                           'consectetur adipiscing elit.')

    # Test: <pre> tag is not supported by docutils.
    # It represented as paragraph with attr pre=1.
    def test_pre(self):
        doc = visitor.parse(dedent("""
        <pre>Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.
        </pre>
        """))

        assert len(doc) == 1
        assert doc[0].get('pre') == '1'
        pytest.g.assert_simple_par(doc[0], 'Lorem ipsum dolor sit amet,\n'
                                           'consectetur adipiscing elit.')
