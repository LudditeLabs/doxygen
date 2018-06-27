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

void PyDocVisitor::visit(DocLinkedWord *node)
{
    printf("visit(DocLinkedWord)\n");
    // TODO: what node to use?
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocSymbol *node)
{
    printf("visit(DocSymbol)\n");
    // TODO: special symbols.
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocURL *node)
{
    printf("visit(DocURL)\n");
    maybeCreateTextNode();

    QCString refuri;
    if (node->isEmail())
        refuri.append("mailto:");
    refuri.append(node->url());

    PyObjectPtr args = PyTuple_New(0);
    PyDict kw;
    kw.setField("refuri", refuri);

    PyObjectPtr ref = m_tree->create("reference", args, kw.get());
    PyObjectPtr txtnode = m_tree->createTextNode(node->url());
    m_tree->addTo(ref, txtnode);
    m_tree->addToCurrent(ref);
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocHorRuler *node)
{
    printf("visit(DocHorRuler)\n");
    // TODO: add <hruler/> like node.
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
