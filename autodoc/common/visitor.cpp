#include "Python.h"
#include "autodoc/common/visitor.h"
#include "autodoc/common/pydocutilstree.h"
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
  m_styled(false)
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

bool PyDocVisitor::maybeCreateTextNode()
{
    if (m_textBuf.isEmpty())
        return false;

    // Remove trailing line break because <paragraph> node already assumes that.
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
    printf("visitPre(DocRoot)\n");

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
    printf("visitPost(DocRoot)\n");
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

void PyDocVisitor::visitPre(DocPara *node)
{
    printf("visitPre(DocPara)\n");
    if (!m_tree->isValid())
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    m_tree->push("paragraph");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocPara *node)
{
    printf("visitPost(DocPara)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop();
}
//-----------------------------------------------------------------------------



void PyDocVisitor::visitPre(DocText *node)
{
    printf("visitPre(DocText)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocText *node)
{
    printf("visitPost(DocText)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocParBlock *node)
{
    printf("visitPre(DocParBlock)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocParBlock *node)
{
    printf("visitPost(DocParBlock)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocAutoList *node)
{
    printf("visitPre(DocAutoList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocAutoList *node)
{
    printf("visitPost(DocAutoList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocAutoListItem *node)
{
    printf("visitPre(DocAutoListItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocAutoListItem *node)
{
    printf("visitPost(DocAutoListItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocTitle *node)
{
    printf("visitPre(DocTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocTitle *node)
{
    printf("visitPost(DocTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSimpleList *node)
{
    printf("visitPre(DocSimpleList)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSimpleList *node)
{
    printf("visitPost(DocSimpleList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSimpleListItem *node)
{
    printf("visitPre(DocSimpleListItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSimpleListItem *node)
{
    printf("visitPost(DocSimpleListItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSection *node)
{
    printf("visitPre(DocSection)\n");

}//-----------------------------------------------------------------------------


void PyDocVisitor::visitPost(DocSection *node)
{
    printf("visitPost(DocSection)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlList *node)
{
    printf("visitPre(DocHtmlList)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlList *node)
{
    printf("visitPost(DocHtmlList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlListItem *node)
{
    printf("visitPre(DocHtmlListItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlListItem *node)
{
    printf("visitPost(DocHtmlListItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlDescList *node)
{
    printf("visitPre(DocHtmlDescList)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlDescList *node)
{
    printf("visitPost(DocHtmlDescList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlDescTitle *node)
{
    printf("visitPre(DocHtmlDescTitle)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlDescTitle *node)
{
    printf("visitPost(DocHtmlDescTitle)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlDescData *node)
{
    printf("visitPre(DocHtmlDescData)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlDescData *node)
{
    printf("visitPost(DocHtmlDescData)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlTable *node)
{
    printf("visitPre(DocHtmlTable)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlTable *node)
{
    printf("visitPost(DocHtmlTable)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlRow *node)
{
    printf("visitPre(DocHtmlRow)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlRow *node)
{
    printf("visitPost(DocHtmlRow)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlCell *node)
{
    printf("visitPre(DocHtmlCell)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlCell *node)
{
    printf("visitPost(DocHtmlCell)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlCaption *node)
{
    printf("visitPre(DocHtmlCaption)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlCaption *node)
{
    printf("visitPost(DocHtmlCaption)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocInternal *node)
{
    printf("visitPre(DocInternal)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocInternal *node)
{
    printf("visitPost(DocInternal)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHRef *node)
{
    printf("visitPre(DocHRef)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHRef *node)
{
    printf("visitPost(DocHRef)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlHeader *node)
{
    printf("visitPre(DocHtmlHeader)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlHeader *node)
{
    printf("visitPost(DocHtmlHeader)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocImage *node)
{
    printf("visitPre(DocImage)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocImage *node)
{
    printf("visitPost(DocImage)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocDotFile *node)
{
    printf("visitPre(DocDotFile)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocDotFile *node)
{
    printf("visitPost(DocDotFile)\n");
}
//-----------------------------------------------------------------------------


void PyDocVisitor::visitPre(DocMscFile *node)
{
    printf("visitPre(DocMscFile)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocMscFile *node)
{
    printf("visitPost(DocMscFile)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocDiaFile *node)
{
    printf("visitPre(DocDiaFile)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocDiaFile *node)
{
    printf("visitPost(DocDiaFile)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocLink *node)
{
    printf("visitPre(DocLink)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocLink *node)
{
    printf("visitPost(DocLink)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocRef *node)
{
    printf("visitPre(DocRef)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocRef *node)
{
    printf("visitPost(DocRef)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSecRefItem *node)
{
    printf("visitPre(DocSecRefItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSecRefItem *node)
{
    printf("visitPost(DocSecRefItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocSecRefList *node)
{
    printf("visitPre(DocSecRefList)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSecRefList *node)
{
    printf("visitPost(DocSecRefList)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocXRefItem *node)
{
    printf("visitPre(DocXRefItem)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocXRefItem *node)
{
    printf("visitPost(DocXRefItem)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocInternalRef *node)
{
    printf("visitPre(DocInternalRef)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocInternalRef *node)
{
    printf("visitPost(DocInternalRef)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocCopy *node)
{
    printf("visitPre(DocCopy)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocCopy *node)
{
    printf("visitPost(DocCopy)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocHtmlBlockQuote *node)
{
    printf("visitPre(DocHtmlBlockQuote)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocHtmlBlockQuote *node)
{
    printf("visitPost(DocHtmlBlockQuote)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocVhdlFlow *node)
{
    printf("visitPre(DocVhdlFlow)\n");

}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocVhdlFlow *node)
{
    printf("visitPost(DocVhdlFlow)\n");
}
//-----------------------------------------------------------------------------
