#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "htmlentity.h"
#include "autodoc/common/docutilstree.h"

// http://docutils.sourceforge.net/docs/ref/rst/restructuredtext.html#bullet-lists

void DocutilsVisitor::visitPre(DocAutoList *node)
{
    TRACE_VISIT("visitPre(DocAutoList)\n");
    if (!beforePre(node))
        return;

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

void DocutilsVisitor::visitPost(DocAutoList *node)
{
    TRACE_VISIT("visitPost(DocAutoList)\n");
    if (!beforePost(node))
        return;
    m_tree->pop();
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocAutoListItem *node)
{
    TRACE_VISIT("visitPre(DocAutoListItem)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->push("list_item");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocAutoListItem *node)
{
    TRACE_VISIT("visitPost(DocAutoListItem)\n");
    if (!beforePost(node))
        return;
    m_tree->pop();
}
//-----------------------------------------------------------------------------


void DocutilsVisitor::visitPre(DocSimpleList *node)
{
    TRACE_VISIT("visitPre(DocSimpleList)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    PyObjectPtr args = PyTuple_New(0);
    PyDict kw;

    kw.setField("bullet", "-");
    kw.setField("simple", "1");
    m_tree->push("bullet_list", args, kw);
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocSimpleList *node)
{
    TRACE_VISIT("visitPost(DocSimpleList)\n");
    if (!beforePost(node))
        return;
    m_tree->pop();
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocSimpleListItem *node)
{
    TRACE_VISIT("visitPre(DocSimpleListItem)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->push("list_item");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocSimpleListItem *node)
{
    TRACE_VISIT("visitPost(DocSimpleListItem)\n");
    if (!beforePost(node))
        return;
    m_tree->pop();
}
//-----------------------------------------------------------------------------


void DocutilsVisitor::visitPre(DocHtmlList *node)
{
    TRACE_VISIT("visitPre(DocHtmlList)\n");
    if (!beforePre(node))
        return;

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

void DocutilsVisitor::visitPost(DocHtmlList *node)
{
    TRACE_VISIT("visitPost(DocHtmlList)\n");
    if (!beforePost(node))
        return;
    m_tree->pop();
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlListItem *node)
{
    TRACE_VISIT("visitPre(DocHtmlListItem)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode(true);
    maybeFinishCurrentPara(node);
    m_tree->push("list_item");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlListItem *node)
{
    TRACE_VISIT("visitPost(DocHtmlListItem)\n");
    if (!beforePost(node))
        return;
    m_tree->pop();
}
//-----------------------------------------------------------------------------

// TODO: implement description list <dl>
// https://www.w3schools.com/tags/tag_dl.asp

void DocutilsVisitor::visitPre(DocHtmlDescList *node)
{
    TRACE_VISIT("visitPre(DocHtmlDescList)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlDescList *node)
{
    TRACE_VISIT("visitPost(DocHtmlDescList)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlDescTitle *node)
{
    TRACE_VISIT("visitPre(DocHtmlDescTitle)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlDescTitle *node)
{
    TRACE_VISIT("visitPost(DocHtmlDescTitle)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlDescData *node)
{
    TRACE_VISIT("visitPre(DocHtmlDescData)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlDescData *node)
{
    TRACE_VISIT("visitPost(DocHtmlDescData)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------
