#include "Python.h"
#include "autodoc/common/visitor.h"
#include "autodoc/common/pydocutilstree.h"
#include "autodoc/common/utils.h"
#include "docparser.h"
#include "config.h"


void pickleDocTree(const QCString &fileName,
                   int lineNr,
                   Definition *scope,
                   MemberDef * md,
                   const QCString &text)
{
    QCString stext = text.stripWhiteSpace();
    if (stext.isEmpty())
    {
        return;
    }

    // convert the documentation string into an abstract syntax tree
    std::unique_ptr<DocNode> root(validatingParseDoc(fileName,lineNr,scope,md,text,FALSE,FALSE));
    std::unique_ptr<PyDocVisitor> visitor(new PyDocVisitor(fileName, lineNr));

    root->accept(visitor.get());

    QCString path = Config_getString(OUTPUT_DIRECTORY);
    PyGlobals::instance()->pickleToFile(visitor->document(), path + "/test.pkl");
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



PyDocVisitor::PyDocVisitor(const QCString &fileName, int lineNumber)
: DocVisitor(DocVisitor_Other),
  m_tree(new PyDocutilsTree),
  m_fileName(fileName),
  m_lineNumber(lineNumber),
  m_skipNextWhitespace(false)
{
}
//-----------------------------------------------------------------------------

PyDocVisitor::~PyDocVisitor()
{
//    m_fieldList.decRef();
}
//-----------------------------------------------------------------------------

PyObject* PyDocVisitor::document() const
{
    return m_tree->document();
}
//-----------------------------------------------------------------------------

PyObject* PyDocVisitor::takeDocument()
{
    return m_tree->takeDocument();
}
//-----------------------------------------------------------------------------

bool PyDocVisitor::maybeCreateTextNode(bool stripTrailing)
{
    if (m_textBuf.isEmpty())
        return false;

    // Remove trailing line break because <paragraph> node already assumes that.
    if (m_tree->currentType() == "paragraph")
        stripTrailing = true;

    if (stripTrailing)
        ::stripTrailing(&m_textBuf);

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

bool PyDocVisitor::maybeFinishCurrentPara(DocNode *node)
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

void PyDocVisitor::visitPre(DocRoot *node)
{
    TRACE_VISIT("visitPre(DocRoot)\n");

//    // We don't have setings and reporter.
//    // document = nodes.document(settings, reporter, source=source_path)

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

void PyDocVisitor::visitPost(DocRoot *node)
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

void PyDocVisitor::visit(DocStyleChange *node)
{
    TRACE_VISIT("visit(DocStyleChange)\n");

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

void PyDocVisitor::visitPre(DocPara *node)
{
    TRACE_VISIT("visitPre(DocPara)\n");
    if (!m_tree->isValid())
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    m_tree->push("paragraph");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocPara *node)
{
    TRACE_VISIT("visitPost(DocPara)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop("paragraph");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocText *)
{
    TRACE_VISIT("visitPre(DocText)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocText *)
{
    TRACE_VISIT("visitPost(DocText)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocParBlock *)
{
    TRACE_VISIT("visitPre(DocParBlock)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocParBlock *)
{
    TRACE_VISIT("visitPost(DocParBlock)\n");
}
//-----------------------------------------------------------------------------


void PyDocVisitor::visitPre(DocTitle *node)
{
    TRACE_VISIT("visitPre(DocTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocTitle *node)
{
    TRACE_VISIT("visitPost(DocTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSection *node)
{
    TRACE_VISIT("visitPre(DocSection)\n");

}//-----------------------------------------------------------------------------


void PyDocVisitor::visitPost(DocSection *node)
{
    TRACE_VISIT("visitPost(DocSection)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlTable *node)
{
    TRACE_VISIT("visitPre(DocHtmlTable)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlTable *node)
{
    TRACE_VISIT("visitPost(DocHtmlTable)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlRow *node)
{
    TRACE_VISIT("visitPre(DocHtmlRow)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlRow *node)
{
    TRACE_VISIT("visitPost(DocHtmlRow)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlCell *node)
{
    TRACE_VISIT("visitPre(DocHtmlCell)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlCell *node)
{
    TRACE_VISIT("visitPost(DocHtmlCell)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlCaption *node)
{
    TRACE_VISIT("visitPre(DocHtmlCaption)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlCaption *node)
{
    TRACE_VISIT("visitPost(DocHtmlCaption)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocInternal *node)
{
    TRACE_VISIT("visitPre(DocInternal)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocInternal *node)
{
    TRACE_VISIT("visitPost(DocInternal)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHRef *node)
{
    TRACE_VISIT("visitPre(DocHRef)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHRef *node)
{
    TRACE_VISIT("visitPost(DocHRef)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlHeader *node)
{
    TRACE_VISIT("visitPre(DocHtmlHeader)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlHeader *node)
{
    TRACE_VISIT("visitPost(DocHtmlHeader)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocImage *node)
{
    TRACE_VISIT("visitPre(DocImage)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocImage *node)
{
    TRACE_VISIT("visitPost(DocImage)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocDotFile *node)
{
    TRACE_VISIT("visitPre(DocDotFile)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocDotFile *node)
{
    TRACE_VISIT("visitPost(DocDotFile)\n");
}
//-----------------------------------------------------------------------------


void PyDocVisitor::visitPre(DocMscFile *node)
{
    TRACE_VISIT("visitPre(DocMscFile)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocMscFile *node)
{
    TRACE_VISIT("visitPost(DocMscFile)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocDiaFile *node)
{
    TRACE_VISIT("visitPre(DocDiaFile)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocDiaFile *node)
{
    TRACE_VISIT("visitPost(DocDiaFile)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocLink *node)
{
    TRACE_VISIT("visitPre(DocLink)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocLink *node)
{
    TRACE_VISIT("visitPost(DocLink)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocRef *node)
{
    TRACE_VISIT("visitPre(DocRef)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocRef *node)
{
    TRACE_VISIT("visitPost(DocRef)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSecRefItem *node)
{
    TRACE_VISIT("visitPre(DocSecRefItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSecRefItem *node)
{
    TRACE_VISIT("visitPost(DocSecRefItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSecRefList *node)
{
    TRACE_VISIT("visitPre(DocSecRefList)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSecRefList *node)
{
    TRACE_VISIT("visitPost(DocSecRefList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocXRefItem *node)
{
    TRACE_VISIT("visitPre(DocXRefItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocXRefItem *node)
{
    TRACE_VISIT("visitPost(DocXRefItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocInternalRef *node)
{
    TRACE_VISIT("visitPre(DocInternalRef)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocInternalRef *node)
{
    TRACE_VISIT("visitPost(DocInternalRef)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocCopy *node)
{
    TRACE_VISIT("visitPre(DocCopy)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocCopy *node)
{
    TRACE_VISIT("visitPost(DocCopy)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlBlockQuote *node)
{
    TRACE_VISIT("visitPre(DocHtmlBlockQuote)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlBlockQuote *node)
{
    TRACE_VISIT("visitPost(DocHtmlBlockQuote)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocVhdlFlow *node)
{
    TRACE_VISIT("visitPre(DocVhdlFlow)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocVhdlFlow *node)
{
    TRACE_VISIT("visitPost(DocVhdlFlow)\n");
}
//-----------------------------------------------------------------------------
