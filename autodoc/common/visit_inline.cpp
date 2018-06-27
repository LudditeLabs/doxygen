#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "autodoc/common/pydocutilstree.h"


void PyDocVisitor::visit(DocWord *node)
{
    printf("visit(DocWord)\n");
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocWhiteSpace *node)
{
    printf("visit(DocWhiteSpace)\n");

    // Add space only if prev node is not style.
    if (!m_styled)
        m_textBuf.append(node->chars());
    m_styled = false;
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
        extra.setField(att->name, att->value);

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
            kw.setField("extra", extra.get());

        switch (node->style())
        {
        case DocStyleChange::Bold:
            tag = "strong";
            break;
        case DocStyleChange::Italic:
            tag = "emphasis";
            break;
        case DocStyleChange::Code:
            // TODO: is it correct tag?
            // doxy: computeroutput
            tag = "literal";
            break;
        case DocStyleChange::Subscript:
            tag = "subscript";
            break;
        case DocStyleChange::Superscript:
            tag = "superscript";
            break;
        case DocStyleChange::Center:
            // NOTE: not supported by docutils, so we wrap with <inline>.
            tag = "inline";
            kw.setField("centered", "1");
            break;
        case DocStyleChange::Small:
            // NOTE: not supported by docutils, so we wrap with <inline>.
            tag = "inline";
            kw.setField("small", "1");
            break;
        case DocStyleChange::Preformatted:
            // NOTE: not supported by docutils, so we wrap with <paragraph>.
            maybeFinishCurrentPara(node);
            tag = "paragraph";
            kw.setField("pre", "1");
            break;
        case DocStyleChange::Div:
            // NOTE: not supported by docutils, so we wrap with <paragraph>.
            maybeFinishCurrentPara(node);
            tag = "paragraph";
            kw.setField("div", "1");
            break;
        case DocStyleChange::Span:
            // NOTE: not supported by docutils, so we wrap with <inline>.
            tag = "inline";
            kw.setField("span", "1");
            break;
        }

        m_tree->push(tag, args.get(), kw.get());
    }

    else
    {
        m_tree->pop();
        m_styled = true;
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
