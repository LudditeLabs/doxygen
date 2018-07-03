#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "htmlentity.h"
#include "autodoc/common/pydocutilstree.h"

// http://docutils.sourceforge.net/docs/ref/rst/restructuredtext.html#bullet-lists

void PyDocVisitor::visitPre(DocAutoList *node)
{
    TRACE_VISIT("visitPre(DocAutoList)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    PyObjectPtr args = PyTuple_New(0);
    PyDict kw;

    if (node->isEnumList())
    {
        kw.setField("enumtype", "arabic");
        kw.setField("suffix", ".");
        m_tree->push("enumerated_list", args, kw);
    }
    else
    {
        kw.setField("bullet", "-");  // TODO: make bullet configurable.
        m_tree->push("bullet_list", args, kw);
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocAutoList *)
{
    TRACE_VISIT("visitPost(DocAutoList)\n");
    m_tree->pop();
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocAutoListItem *node)
{
    TRACE_VISIT("visitPre(DocAutoListItem)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->push("list_item");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocAutoListItem *)
{
    TRACE_VISIT("visitPost(DocAutoListItem)\n");
    m_tree->pop();
}
//-----------------------------------------------------------------------------


void PyDocVisitor::visitPre(DocSimpleList *node)
{
    TRACE_VISIT("visitPre(DocSimpleList)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    PyObjectPtr args = PyTuple_New(0);
    PyDict kw;

    kw.setField("bullet", "-");
    kw.setField("simple", "1");
    m_tree->push("bullet_list", args, kw);
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSimpleList *)
{
    TRACE_VISIT("visitPost(DocSimpleList)\n");
    m_tree->pop();
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSimpleListItem *node)
{
    TRACE_VISIT("visitPre(DocSimpleListItem)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->push("list_item");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSimpleListItem *)
{
    TRACE_VISIT("visitPost(DocSimpleListItem)\n");
    m_tree->pop();
}
//-----------------------------------------------------------------------------


void PyDocVisitor::visitPre(DocHtmlList *node)
{
    TRACE_VISIT("visitPre(DocHtmlList)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    PyObjectPtr args = PyTuple_New(0);
    PyDict kw;

    kw.setField("html", "1");
    if (node->type())
    {
        kw.setField("enumtype", "arabic");
        kw.setField("suffix", ".");
        m_tree->push("enumerated_list", args, kw);
    }
    else
    {
        kw.setField("bullet", "-");
        m_tree->push("bullet_list", args, kw);
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlList *)
{
    TRACE_VISIT("visitPost(DocHtmlList)\n");
    m_tree->pop();
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlListItem *node)
{
    TRACE_VISIT("visitPre(DocHtmlListItem)\n");
    maybeCreateTextNode(true);
    maybeFinishCurrentPara(node);
    m_tree->push("list_item");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlListItem *)
{
    TRACE_VISIT("visitPost(DocHtmlListItem)\n");
    m_tree->pop();
}
//-----------------------------------------------------------------------------

// TODO: implement description list <dl>
// https://www.w3schools.com/tags/tag_dl.asp

void PyDocVisitor::visitPre(DocHtmlDescList *node)
{
    TRACE_VISIT("visitPre(DocHtmlDescList)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlDescList *node)
{
    TRACE_VISIT("visitPost(DocHtmlDescList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlDescTitle *node)
{
    TRACE_VISIT("visitPre(DocHtmlDescTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlDescTitle *node)
{
    TRACE_VISIT("visitPost(DocHtmlDescTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlDescData *node)
{
    TRACE_VISIT("visitPre(DocHtmlDescData)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlDescData *node)
{
    TRACE_VISIT("visitPost(DocHtmlDescData)\n");
}
//-----------------------------------------------------------------------------
