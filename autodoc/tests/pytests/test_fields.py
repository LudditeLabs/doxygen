import pytest
from textwrap import dedent
from docutils import nodes
import visitor


# Test: Commands generating field nodes.
class TestFields:
    # Test: simple fields.
    @pytest.mark.parametrize('docstring,name,paragrahp', [
        # @return, @returns
        ("""
         First line.
         @return Lorem ipsum dolor sit amet,
                 consectetur adipiscing elit

         Last line.
         """,
         'return',
         'Lorem ipsum dolor sit amet,\n        consectetur adipiscing elit'
        ),
        ("""
         First line.
         @returns Lorem ipsum dolor sit amet,
                  consectetur adipiscing elit

         Last line.
         """,
         'return',
         'Lorem ipsum dolor sit amet,\n         consectetur adipiscing elit'
        ),

        # @author, @authors
        ("""
         First line.

         @author Lorem ipsum dolor sit amet,
         consectetur adipiscing elit

         Last line.
         """,
         'Author',
         'Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit'
        ),
        ("""
         First line.

         @authors Lorem ipsum dolor sit amet,
         consectetur adipiscing elit

         Last line.
         """,
         'Authors',
         'Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit'
        ),

        # @version
        ("""
         First line.
         @version 1.23 Lorem ipsum

         Last line.
         """,
         'Version',
         '1.23 Lorem ipsum'
        ),

        # @date
        ("""
         First line.
         @date Lorem ipsum

         Last line.
         """,
         'Date',
         'Lorem ipsum'
        ),

        # @copyright
        ("""
         First line.
         @copyright Lorem ipsum

         Last line.
         """,
         'copyright',
         'Lorem ipsum'
        ),
    ])
    def test_simple(self, docstring, name, paragrahp):
        doc = visitor.parse(dedent(docstring))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        field = doc[1]
        assert isinstance(field, nodes.field)
        assert len(field) == 2

        field_name = field[0]
        assert isinstance(field_name, nodes.field_name)
        assert len(field_name) == 1
        assert isinstance(field_name[0], nodes.Text)
        assert len(field_name[0].children) == 0
        assert str(field_name[0]) == name

        body = field[1]
        assert isinstance(body, nodes.field_body)
        assert len(body) == 1
        pytest.g.assert_simple_par(body[0], paragrahp)

    # Test: RCS (Revision Control System, CSV, perforce) keyword: $<ID>:<text>$
    def test_rcs(self):
        doc = visitor.parse(dedent("""
        First line.
        $Author: Lorem ipsum$

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        field = doc[1]
        assert isinstance(field, nodes.field)
        assert field.get('rcs') == '1'
        assert len(field) == 2

        field_name = field[0]
        assert isinstance(field_name, nodes.field_name)
        assert len(field_name) == 1
        assert isinstance(field_name[0], nodes.Text)
        assert len(field_name[0].children) == 0
        assert str(field_name[0]) == 'Author'

        body = field[1]
        assert isinstance(body, nodes.field_body)
        assert len(body) == 1
        pytest.g.assert_simple_par(body[0], 'Lorem ipsum')


class TestDirectives:
    # Test: Simple case @since -> admonition(type='versionadded', version=...)
    def test_since_simple(self):
        doc = visitor.parse(dedent("""
        First line.
        @since 2.3.4

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.admonition)
        assert len(node) == 0

        assert node.get('type') == 'versionadded'
        assert node.get('version') == '2.3.4'

    # Test: @since with paragraph
    def test_since_par(self):
        doc = visitor.parse(dedent("""
        First line.
        @since 2.3.4 привет Lorem ipsum dolor sit amet,
        consectetur adipiscing elit

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.admonition)
        assert len(node) == 1

        pytest.g.assert_simple_par(node[0], 'consectetur adipiscing elit')
        assert node.get('type') == 'versionadded'
        assert node.get('version') == '2.3.4 привет Lorem ipsum dolor sit amet,'

    # Test: @see, @sa
    def test_seealso(self):
        doc = visitor.parse(dedent("""
        First line.
        @see myFunc(), ::name, Class::foo.

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.admonition)
        assert len(node) == 1
        assert node.get('type') == 'seealso'
        pytest.g.assert_simple_par(node[0], 'myFunc(), ::name, Class::foo.')

    # Test: @attention
    def test_attention(self):
        doc = visitor.parse(dedent("""
        First line.
        @attention
        Lorem ipsum dolor sit amet

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.admonition)
        assert len(node) == 1
        assert node.get('type') == 'attention'
        pytest.g.assert_simple_par(node[0], 'Lorem ipsum dolor sit amet')

    # Test: @note
    def test_note(self):
        doc = visitor.parse(dedent("""
        First line.
        @note Lorem ipsum dolor sit amet
        
        Last line.
        """))
        assert len(doc) == 3
        
        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')
        
        node = doc[1]
        assert isinstance(node, nodes.note)
        assert len(node) == 1
        pytest.g.assert_simple_par(node[0], 'Lorem ipsum dolor sit amet')

    # Test: @warning
    def test_warning(self):
        doc = visitor.parse(dedent("""
        First line.
        @warning Lorem ipsum dolor sit amet

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.warning)
        assert len(node) == 1
        pytest.g.assert_simple_par(node[0], 'Lorem ipsum dolor sit amet')

    # Test: @pre
    def test_pre(self):
        doc = visitor.parse(dedent("""
        First line.
        @pre Lorem ipsum dolor sit amet

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.paragraph)
        pytest.g.assert_simple_par(node, 'Lorem ipsum dolor sit amet')
        assert node.get('type') == 'pre'

    # Test: @post
    def test_post(self):
        doc = visitor.parse(dedent("""
        First line.
        @post
        Lorem ipsum dolor sit amet

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.paragraph)
        pytest.g.assert_simple_par(node, 'Lorem ipsum dolor sit amet')
        assert node.get('type') == 'post'

    # Test: @invariant
    def test_invariant(self):
        doc = visitor.parse(dedent("""
        First line.
        @invariant i+j=p

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.paragraph)
        pytest.g.assert_simple_par(node, 'i+j=p')
        assert node.get('type') == 'invariant'

    # Test: @remark
    def test_remark(self):
        doc = visitor.parse(dedent("""
        First line.
        @remark Lorem ipsum dolor sit amet,
        consectetur adipiscing elit.

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.paragraph)
        pytest.g.assert_simple_par(node, 'Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit.')
        assert node.get('type') == 'remark'

    # Test: paragraph with user defined heading.
    def test_par_title(self):
        doc = visitor.parse(dedent("""
        First line.
        @par User defined paragraph:
        Contents of the paragraph.
        next line.

        Last line.
        """))
        assert len(doc) == 3

        pytest.g.assert_simple_par(doc[0], 'First line.')
        pytest.g.assert_simple_par(doc[2], 'Last line.')

        node = doc[1]
        assert isinstance(node, nodes.paragraph)
        assert len(node) == 2

        # first node is title.
        n = node[0]
        assert isinstance(n, nodes.inline)
        assert n.get('type') == 'title'
        assert len(n) == 1
        n = n[0]
        assert isinstance(n, nodes.Text)
        assert str(n) == 'User defined paragraph:'

        # second node is content.
        n = node[1]
        assert isinstance(n, nodes.Text)
        assert str(n) == 'Contents of the paragraph.\nnext line.'


