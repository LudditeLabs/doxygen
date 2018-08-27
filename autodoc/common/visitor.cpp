#include "autodoc/common/visitor.h"
#include "autodoc/common/docutilstree.h"
#include "autodoc/common/utils.h"
#include "docparser.h"
#include "config.h"

namespace autodoc {

PyObject* pickleDocTree(const QCString &fileName,
                        int lineNr,
                        Definition *scope,
                        MemberDef * md,
                        const QCString &text)
{
    QCString stext = text.stripWhiteSpace();
    if (stext.isEmpty())
        return NULL;

    // convert the documentation string into an abstract syntax tree
    std::unique_ptr<DocNode> root(validatingParseDoc(fileName,lineNr,scope,md,text,FALSE,FALSE));
    std::unique_ptr<DocutilsVisitor> visitor(new DocutilsVisitor(fileName, lineNr));

    root->accept(visitor.get());

    QCString path = Config_getString(OUTPUT_DIRECTORY);
    return PyGlobals::instance()->pickleToString(visitor->document());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


DocutilsVisitor::DocutilsVisitor(const QCString &fileName, int lineNumber)
: DocVisitor(DocVisitor_Other),
  m_tree(new DocutilsTree),
  m_fileName(fileName),
  m_lineNumber(lineNumber),
  m_skipNextWhitespace(false),
  m_skipProcessing(-1),
  m_titleParent(-1)
{
}
//-----------------------------------------------------------------------------

DocutilsVisitor::~DocutilsVisitor()
{
//    m_fieldList.decRef();
}
//-----------------------------------------------------------------------------

PyObject* DocutilsVisitor::document() const
{
    return m_tree->document();
}
//-----------------------------------------------------------------------------

PyObject* DocutilsVisitor::takeDocument()
{
    return m_tree->takeDocument();
}
//-----------------------------------------------------------------------------

bool DocutilsVisitor::maybeCreateTextNode(bool stripTrailing)
{
    if (m_textBuf.isEmpty())
        return false;

    // Remove trailing line break because <paragraph> node already assumes that.
    if (m_tree->currentType() == "paragraph")
        stripTrailing = true;

    if (stripTrailing)
        autodoc::stripTrailing(&m_textBuf);

    if (m_tree->currentType() == "paragraph")
    {
        if (m_textBuf.at(m_textBuf.size() - 1) == '\n')
            m_textBuf.remove(m_textBuf.size() - 1, 1);
    }

    if (m_textBuf.isEmpty())
        return false;

    PyObjectPtr pynode = m_tree->createTextNode(m_textBuf);
    m_textBuf = QCString();

    if (!pynode)
        return false;
    return m_tree->addToCurrent(pynode);
}
//-----------------------------------------------------------------------------

bool DocutilsVisitor::maybeFinishCurrentPara(DocNode *node)
{
    if (node->kind() == DocNode::Kind_Para)
    {
        if (m_tree->parentType() == "paragraph")
        {
            m_tree->pop();
            return true;
        }
    }
    else if (m_tree->currentType() == "paragraph")
    {
        Py_ssize_t sz = m_tree->len(m_tree->current());
        if (sz == 0)
        {
            PyObjectPtr parent = m_tree->getParentOf(m_tree->current());
            sz = m_tree->len(parent);
            if (sz > 0)
                m_tree->removeChild(parent, sz - 1);
        }
        m_tree->pop();
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------

bool DocutilsVisitor::beforePre(DocNode *node)
{
    if (m_skipProcessing != -1 || !m_tree->isValid())
        return false;

    return true;
}
//-----------------------------------------------------------------------------

bool DocutilsVisitor::beforePost(DocNode *node)
{
    if (m_skipProcessing != -1)
        return false;
    return true;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocRoot *node)
{
    TRACE_VISIT("visitPre(DocRoot)\n");

    // We don't have setings and reporter.
    // document = nodes.document(settings, reporter, source=source_path)

    PyDict kw;
    kw.setField("source", m_fileName);

    PyObject *doc = m_tree->createDocument(kw.get());

    if (doc)
    {
        // document.current_source = <m_lineNumber>
        // Line number (1-based) of `current_source`.
        PyObjectPtr val = PyLong_FromLong(m_lineNumber + 1);
        if (PyObject_SetAttrString(doc, "current_source", val.get()) == -1)
            printPyError();
    }
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocRoot *node)
{
    TRACE_VISIT("visitPost(DocRoot)\n");

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop();
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

void DocutilsVisitor::visit(DocStyleChange *node)
{
    TRACE_VISIT("visit(DocStyleChange)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode(true);

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
        case DocStyleChange::Strike:
            // NOTE: not supported by docutils, so we wrap with <inline>.
            tag = "inline";
            kw.setField("strike", "1");
            break;
        case DocStyleChange::Underline:
            // NOTE: not supported by docutils, so we wrap with <inline>.
            tag = "inline";
            kw.setField("underline", "1");
            break;
        }

        m_tree->push(tag, args.get(), kw.get());
    }

    else
    {
        m_tree->pop();
        m_skipNextWhitespace = true;
    }
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocPara *node)
{
    TRACE_VISIT("visitPre(DocPara)\n");
    if (!beforePre(node))
        return;

    if (!m_tree->isValid())
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    m_tree->push("paragraph");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocPara *node)
{
    TRACE_VISIT("visitPost(DocPara)\n");
    if (!beforePost(node))
        return;


    // If there is some text and current node is not a paragraph
    // then insert paragraph to wrap the text.
    if (m_tree->currentType() != "paragraph" && !m_textBuf.isEmpty())
        m_tree->push("paragraph");

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    // pop only paragraph node.
    // If text is empty and current node is not a paragraph then someone
    // already performed all 'pops'.
    m_tree->pop("paragraph");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocText *node)
{
    TRACE_VISIT("visitPre(DocText)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocText *node)
{
    TRACE_VISIT("visitPost(DocText)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocParBlock *node)
{
    TRACE_VISIT("visitPre(DocParBlock)\n");
    if (!beforePre(node))
        return;

}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocParBlock *node)
{
    TRACE_VISIT("visitPost(DocParBlock)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocSection *node)
{
    TRACE_VISIT("visitPre(DocSection)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocSection *node)
{
    TRACE_VISIT("visitPost(DocSection)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlTable *node)
{
    TRACE_VISIT("visitPre(DocHtmlTable)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlTable *node)
{
    TRACE_VISIT("visitPost(DocHtmlTable)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlRow *node)
{
    TRACE_VISIT("visitPre(DocHtmlRow)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlRow *node)
{
    TRACE_VISIT("visitPost(DocHtmlRow)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlCell *node)
{
    TRACE_VISIT("visitPre(DocHtmlCell)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlCell *node)
{
    TRACE_VISIT("visitPost(DocHtmlCell)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlCaption *node)
{
    TRACE_VISIT("visitPre(DocHtmlCaption)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlCaption *node)
{
    TRACE_VISIT("visitPost(DocHtmlCaption)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocInternal *node)
{
    TRACE_VISIT("visitPre(DocInternal)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocInternal *node)
{
    TRACE_VISIT("visitPost(DocInternal)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHRef *node)
{
    TRACE_VISIT("visitPre(DocHRef)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHRef *node)
{
    TRACE_VISIT("visitPost(DocHRef)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlHeader *node)
{
    TRACE_VISIT("visitPre(DocHtmlHeader)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlHeader *node)
{
    TRACE_VISIT("visitPost(DocHtmlHeader)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocImage *node)
{
    TRACE_VISIT("visitPre(DocImage)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocImage *node)
{
    TRACE_VISIT("visitPost(DocImage)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocDotFile *node)
{
    TRACE_VISIT("visitPre(DocDotFile)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocDotFile *node)
{
    TRACE_VISIT("visitPost(DocDotFile)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------


void DocutilsVisitor::visitPre(DocMscFile *node)
{
    TRACE_VISIT("visitPre(DocMscFile)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocMscFile *node)
{
    TRACE_VISIT("visitPost(DocMscFile)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocDiaFile *node)
{
    TRACE_VISIT("visitPre(DocDiaFile)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocDiaFile *node)
{
    TRACE_VISIT("visitPost(DocDiaFile)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocLink *node)
{
    TRACE_VISIT("visitPre(DocLink)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocLink *node)
{
    TRACE_VISIT("visitPost(DocLink)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocRef *node)
{
    TRACE_VISIT("visitPre(DocRef)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocRef *node)
{
    TRACE_VISIT("visitPost(DocRef)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocSecRefItem *node)
{
    TRACE_VISIT("visitPre(DocSecRefItem)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocSecRefItem *node)
{
    TRACE_VISIT("visitPost(DocSecRefItem)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocSecRefList *node)
{
    TRACE_VISIT("visitPre(DocSecRefList)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocSecRefList *node)
{
    TRACE_VISIT("visitPost(DocSecRefList)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocXRefItem *node)
{
    TRACE_VISIT("visitPre(DocXRefItem)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocXRefItem *node)
{
    TRACE_VISIT("visitPost(DocXRefItem)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocInternalRef *node)
{
    TRACE_VISIT("visitPre(DocInternalRef)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocInternalRef *node)
{
    TRACE_VISIT("visitPost(DocInternalRef)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocCopy *node)
{
    TRACE_VISIT("visitPre(DocCopy)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocCopy *node)
{
    TRACE_VISIT("visitPost(DocCopy)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocHtmlBlockQuote *node)
{
    TRACE_VISIT("visitPre(DocHtmlBlockQuote)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocHtmlBlockQuote *node)
{
    TRACE_VISIT("visitPost(DocHtmlBlockQuote)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocVhdlFlow *node)
{
    TRACE_VISIT("visitPre(DocVhdlFlow)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocVhdlFlow *node)
{
    TRACE_VISIT("visitPost(DocVhdlFlow)\n");
    if (!beforePost(node))
        return;
}
//-----------------------------------------------------------------------------

} // namespace autodoc
