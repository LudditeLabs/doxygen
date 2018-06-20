#include "Python.h"
#include "autodoc/visitor/visitor.h"
#include <memory>
#include "docparser.h"
#include "autodoc/pynode.h"


void PyDocVisitor::visit(DocWord *node)
{
    printf("visit(DocWord)\n");
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocWhiteSpace *node)
{
    printf("visit(DocWhiteSpace)\n");
    m_textBuf.append(node->chars());
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocLineBreak *node)
{
    printf("visit(DocLineBreak)\n");
    m_textBuf.append("\n");
}
//-----------------------------------------------------------------------------

// TODO: test me!
static PyObject* htmlAttribsToDict(const HtmlAttribList &attribs)
{
    if (attribs.isEmpty())
        return NULL;

    PyDict extra;

    HtmlAttribListIterator li(attribs);
    HtmlAttrib *att;

    for (li.toFirst(); (att = li.current()); ++li)
    {
        if (!att->value.isEmpty())
            extra.setField(att->name, att->value);
        // NOTE: we add even empty attrs.
        else
        {
            Py_INCREF(Py_None);
            extra.setField(att->name, Py_None);
            Py_DECREF(Py_None);
        }
    }

    return extra.take();
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocStyleChange *node)
{
    printf("visit(DocStyleChange)\n");

    maybeCreateTextNode();

    if (node->enable())
    {
        PyObjectPtr args = PyTuple_New(0);
        PyDict kw;
        QCString tag;

        PyObjectPtr extra = htmlAttribsToDict(node->attribs());
        if (extra)
            kw.setField("attribs", extra);

        switch (node->style())
        {
        case DocStyleChange::Bold:
            tag = "strong";
            break;
        case DocStyleChange::Italic:
            tag = "emphasis";
            break;
        case DocStyleChange::Code:
            // doxy: computeroutput
            // TODO: it also could be 'code' block.
            // http://docutils.sourceforge.net/docs/ref/rst/directives.html#code
            tag = "literal_block";
            kw.setField("realtag", "code");
            break;
        case DocStyleChange::Subscript:
            tag = "subscript";
            break;
        case DocStyleChange::Superscript:
            tag = "superscript";
            break;
        case DocStyleChange::Center:
            tag = "paragraph";
            kw.setField("realtag", "center");
            break;
        case DocStyleChange::Small:
            tag = "inline";
            kw.setField("realtag", "small");
            break;
        case DocStyleChange::Preformatted:
            // doxy: preformatted
            // TODO: or 'code' block?
            // http://docutils.sourceforge.net/docs/ref/rst/directives.html#code
            tag = "literal_block";
            kw.setField("realtag", "preformatted");
            break;
        case DocStyleChange::Div:
            tag = "paragraph";
            kw.setField("realtag", "div");  // TODO: html tag?
            break;
        case DocStyleChange::Span:
            tag = "inline";
            kw.setField("realtag", "span");
            break;
        }

        m_tree->push(tag, args.get(), kw.get());
    }

    else
    {
        m_tree->pop();
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocLinkedWord *node)
{
    printf("visit(DocLinkedWord)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocSymbol *node)
{
    printf("visit(DocSymbol)\n");
}
//-----------------------------------------------------------------------------


void PyDocVisitor::visit(DocURL *node)
{
    printf("visit(DocURL)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocHorRuler *node)
{
    printf("visit(DocHorRuler)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocVerbatim *node)
{
    printf("visit(DocVerbatim)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocAnchor *node)
{
    printf("visit(DocAnchor)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocInclude *node)
{
    printf("visit(DocInclude)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocIncOperator *node)
{
    printf("visit(DocIncOperator)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocFormula *node)
{
    printf("visit(DocFormula)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocIndexEntry *node)
{
    printf("visit(DocIndexEntry)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocSimpleSectSep *node)
{
    printf("visit(DocSimpleSectSep)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocCite *node)
{
    printf("visit(DocCite)\n");
}
//-----------------------------------------------------------------------------
