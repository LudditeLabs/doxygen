import pytest
from textwrap import dedent
from docutils import nodes
import visitor


# Test: parse empty docstring.
def test_empty_document():
    doc = visitor.parse("")
    assert isinstance(doc, nodes.document)
    assert len(doc.children) == 0
