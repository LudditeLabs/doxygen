import pytest
from textwrap import dedent
from docutils import nodes
import visitor


# Test: Commands generating field nodes.
class TestParams:
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
