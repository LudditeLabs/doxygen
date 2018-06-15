#include "Python.h"
#include <memory>
#include "docparser.h"
#include "autodoc/pickledocvisitor.h"
#include "autodoc/pynode.h"


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
    std::unique_ptr<PickleDocVisitor> visitor(new PickleDocVisitor(fileName, lineNr));

    root->accept(visitor.get());

    //PyNode::instance()->pickle(visitor->document(), "test.pkl");
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


PickleDocVisitor::PickleDocVisitor(const QCString &fileName, int lineNumber)
    : DocVisitor(DocVisitor_Other),
      m_tree(new PyDocutilsTree),
      m_fileName(fileName),
      m_lineNumber(lineNumber)
{
}

PickleDocVisitor::~PickleDocVisitor()
{

}

PyObject* PickleDocVisitor::document() const
{
    return m_tree->document();
}

void PickleDocVisitor::visit(DocWord *node)
{
    printf("visit(DocWord)\n");
    m_textBuf.append(node->word());
}

void PickleDocVisitor::visit(DocWhiteSpace *node)
{
    printf("visit(DocWhiteSpace)\n");
    m_textBuf.append(node->chars());
}

void PickleDocVisitor::visit(DocLineBreak *node)
{
    printf("visit(DocLineBreak)\n");
    m_textBuf.append("\n");
}

bool PickleDocVisitor::maybeCreateTextNode()
{
    if (m_textBuf.isEmpty())
        return false;

    PyObjectPtr pynode = m_tree->createTextNode(m_textBuf);
    m_textBuf = QCString();

    if (!pynode)
        return false;
    return m_tree->addToCurrent(pynode);
}

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

void PickleDocVisitor::visit(DocStyleChange *node)
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

void PickleDocVisitor::visit(DocLinkedWord *node)
{
    printf("visit(DocLinkedWord)\n");
}

void PickleDocVisitor::visit(DocSymbol *node)
{
    printf("visit(DocSymbol)\n");
}


void PickleDocVisitor::visit(DocURL *node)
{
    printf("visit(DocURL)\n");
}

void PickleDocVisitor::visit(DocHorRuler *node)
{
    printf("visit(DocHorRuler)\n");
}

void PickleDocVisitor::visit(DocVerbatim *node)
{
    printf("visit(DocVerbatim)\n");
}

void PickleDocVisitor::visit(DocAnchor *node)
{
    printf("visit(DocAnchor)\n");
}

void PickleDocVisitor::visit(DocInclude *node)
{
    printf("visit(DocInclude)\n");
}

void PickleDocVisitor::visit(DocIncOperator *node)
{
    printf("visit(DocIncOperator)\n");
}

void PickleDocVisitor::visit(DocFormula *node)
{
    printf("visit(DocFormula)\n");
}

void PickleDocVisitor::visit(DocIndexEntry *node)
{
    printf("visit(DocIndexEntry)\n");
}

void PickleDocVisitor::visit(DocSimpleSectSep *node)
{
    printf("visit(DocSimpleSectSep)\n");
}

void PickleDocVisitor::visit(DocCite *node)
{
    printf("visit(DocCite)\n");
}


//--------------------------------------
// visitor functions for compound nodes
//--------------------------------------

void PickleDocVisitor::visitPre(DocRoot *node)
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
        {
            PyErr_Print();
        }
    }
}

void PickleDocVisitor::visitPost(DocRoot *node)
{
    printf("visitPost(DocRoot)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop();
}

bool PickleDocVisitor::maybeFinishCurrentPara(DocNode *node)
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
        m_tree->pop();
        return true;
    }
    return false;
}

bool PickleDocVisitor::beforeVisit(DocNode *node)
{
    if (!m_tree->isValid())
        return false;

    return true;
}

void PickleDocVisitor::visitPre(DocPara *node)
{
    printf("visitPre(DocPara)\n");
    if (!m_tree->isValid())
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    m_tree->push("paragraph");
}

void PickleDocVisitor::visitPost(DocPara *node)
{
    printf("visitPost(DocPara)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop();
}

void PickleDocVisitor::visitPre(DocParamSect *node)
{
    printf("visitPre(DocParamSect)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    m_paramType = node->type();
    m_tree->push("field_list");
}

void PickleDocVisitor::visitPost(DocParamSect *node)
{
    printf("visitPost(DocParamSect)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop();
}

void PickleDocVisitor::visitPre(DocParamList *node)
{
    printf("visitPre(DocParamList)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    // Create tuple with parameter types (optional).
    PyTuple type_list(node->paramTypes().count());
    if (!node->paramTypes().isEmpty())
    {
        QListIterator<DocNode> it(node->paramTypes());
        DocNode *type;
        int i = 0;
        for (it.toFirst(); (type=it.current()); ++it, ++i)
        {
            if (type->kind()==DocNode::Kind_Word)
                type_list.add(static_cast<DocWord*>(type)->word());
            else if (type->kind()==DocNode::Kind_LinkedWord)
                type_list.add(static_cast<DocLinkedWord*>(type)->word());
            else
                type_list.add("N/A");  // Should not happen.
        }
    }

    // Get parameter direction (optional).
    QCString direction;
    switch (node->direction()) {
    case DocParamSect::In:
        direction = "in";
        break;
    case DocParamSect::Out:
        direction = "out";
        break;
    case DocParamSect::InOut:
        direction = "inout";
        break;
    default:
        break;
    }

    // Get parameter name.
    QCString name;
    PyTuple other_names(node->parameters().count() - 1);
    DocNode *param;
    QListIterator<DocNode> it(node->parameters());
    for (it.toFirst(); (param = it.current()); ++it)
    {
        QCString val;
        if (param->kind() == DocNode::Kind_Word)
            val = static_cast<DocWord*>(param)->word();
        else if (param->kind() == DocNode::Kind_LinkedWord)
            val = static_cast<DocLinkedWord*>(param)->word();

        if (name.isNull())
            name = val;
        else
            other_names.add(val);
    }

    PyObjectPtr args = PyTuple_New(0);

    PyDict kw;
    if (!direction.isEmpty())
        kw.setField("direction", direction);

    m_tree->push("field", args.get(), kw.get());

    PyDict fieldkw;  // optional names.
    if (!other_names.isNull())
        fieldkw.setField("names", other_names.get());

    if(m_paramType == DocParamSect::Param) {
        name = "param " + name;
    }
    else if (m_paramType == DocParamSect::TemplateParam)
    {
        name = "param " + name;
        Py_INCREF(Py_True);
        fieldkw.setField("is_template_param", Py_True);
        Py_DECREF(Py_True);
    }
    else if (m_paramType == DocParamSect::Exception)
    {
        // TODO: create raises field.
        name = "raises";
    }

    else if (m_paramType == DocParamSect::RetVal)
    {
        // TODO: implement me! \retval
        // Add this info to 'returns' field.
        name = "returns";
    }
    else
    {
        // TODO: what to do?
    }

    PyObjectPtr text = m_tree->createTextNode(name);
    PyObjectPtr field_name = m_tree->create("field_name", args.get(), fieldkw.get());
    m_tree->addTo(field_name, text);
    m_tree->addTo(m_tree->current(), field_name);

    m_tree->push("field_body");
}

void PickleDocVisitor::visitPost(DocParamList *node)
{
    printf("visitPost(DocParamList)\n");
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    // field_body -> field.
    m_tree->pop();
    // field -> field_list.
    m_tree->pop();
}

void PickleDocVisitor::visitPre(DocText *node)
{
    printf("visitPre(DocText)\n");

}

void PickleDocVisitor::visitPost(DocText *node)
{
    printf("visitPost(DocText)\n");
}

void PickleDocVisitor::visitPre(DocParBlock *node)
{
    printf("visitPre(DocParBlock)\n");

}

void PickleDocVisitor::visitPost(DocParBlock *node)
{
    printf("visitPost(DocParBlock)\n");
}


void PickleDocVisitor::visitPre(DocAutoList *node)
{
    printf("visitPre(DocAutoList)\n");
}

void PickleDocVisitor::visitPost(DocAutoList *node)
{
    printf("visitPost(DocAutoList)\n");
}

void PickleDocVisitor::visitPre(DocAutoListItem *node)
{
    printf("visitPre(DocAutoListItem)\n");

}

void PickleDocVisitor::visitPost(DocAutoListItem *node)
{
    printf("visitPost(DocAutoListItem)\n");
}

void PickleDocVisitor::visitPre(DocSimpleSect *node)
{
    printf("visitPre(DocSimpleSect)\n");

}

void PickleDocVisitor::visitPost(DocSimpleSect *node)
{
    printf("visitPost(DocSimpleSect)\n");
}

void PickleDocVisitor::visitPre(DocTitle *node)
{
    printf("visitPre(DocTitle)\n");

}

void PickleDocVisitor::visitPost(DocTitle *node)
{
    printf("visitPost(DocTitle)\n");
}

void PickleDocVisitor::visitPre(DocSimpleList *node)
{
    printf("visitPre(DocSimpleList)\n");

}

void PickleDocVisitor::visitPost(DocSimpleList *node)
{
    printf("visitPost(DocSimpleList)\n");
}

void PickleDocVisitor::visitPre(DocSimpleListItem *node)
{
    printf("visitPre(DocSimpleListItem)\n");

}

void PickleDocVisitor::visitPost(DocSimpleListItem *node)
{
    printf("visitPost(DocSimpleListItem)\n");
}

void PickleDocVisitor::visitPre(DocSection *node)
{
    printf("visitPre(DocSection)\n");

}

void PickleDocVisitor::visitPost(DocSection *node)
{
    printf("visitPost(DocSection)\n");
}

void PickleDocVisitor::visitPre(DocHtmlList *node)
{
    printf("visitPre(DocHtmlList)\n");

}

void PickleDocVisitor::visitPost(DocHtmlList *node)
{
    printf("visitPost(DocHtmlList)\n");
}

void PickleDocVisitor::visitPre(DocHtmlListItem *node)
{
    printf("visitPre(DocHtmlListItem)\n");

}

void PickleDocVisitor::visitPost(DocHtmlListItem *node)
{
    printf("visitPost(DocHtmlListItem)\n");
}

void PickleDocVisitor::visitPre(DocHtmlDescList *node)
{
    printf("visitPre(DocHtmlDescList)\n");

}

void PickleDocVisitor::visitPost(DocHtmlDescList *node)
{
    printf("visitPost(DocHtmlDescList)\n");
}

void PickleDocVisitor::visitPre(DocHtmlDescTitle *node)
{
    printf("visitPre(DocHtmlDescTitle)\n");

}

void PickleDocVisitor::visitPost(DocHtmlDescTitle *node)
{
    printf("visitPost(DocHtmlDescTitle)\n");
}

void PickleDocVisitor::visitPre(DocHtmlDescData *node)
{
    printf("visitPre(DocHtmlDescData)\n");

}

void PickleDocVisitor::visitPost(DocHtmlDescData *node)
{
    printf("visitPost(DocHtmlDescData)\n");
}

void PickleDocVisitor::visitPre(DocHtmlTable *node)
{
    printf("visitPre(DocHtmlTable)\n");

}

void PickleDocVisitor::visitPost(DocHtmlTable *node)
{
    printf("visitPost(DocHtmlTable)\n");
}

void PickleDocVisitor::visitPre(DocHtmlRow *node)
{
    printf("visitPre(DocHtmlRow)\n");

}

void PickleDocVisitor::visitPost(DocHtmlRow *node)
{
    printf("visitPost(DocHtmlRow)\n");
}

void PickleDocVisitor::visitPre(DocHtmlCell *node)
{
    printf("visitPre(DocHtmlCell)\n");

}

void PickleDocVisitor::visitPost(DocHtmlCell *node)
{
    printf("visitPost(DocHtmlCell)\n");
}

void PickleDocVisitor::visitPre(DocHtmlCaption *node)
{
    printf("visitPre(DocHtmlCaption)\n");

}

void PickleDocVisitor::visitPost(DocHtmlCaption *node)
{
    printf("visitPost(DocHtmlCaption)\n");
}

void PickleDocVisitor::visitPre(DocInternal *node)
{
    printf("visitPre(DocInternal)\n");

}

void PickleDocVisitor::visitPost(DocInternal *node)
{
    printf("visitPost(DocInternal)\n");
}

void PickleDocVisitor::visitPre(DocHRef *node)
{
    printf("visitPre(DocHRef)\n");

}

void PickleDocVisitor::visitPost(DocHRef *node)
{
    printf("visitPost(DocHRef)\n");
}

void PickleDocVisitor::visitPre(DocHtmlHeader *node)
{
    printf("visitPre(DocHtmlHeader)\n");

}

void PickleDocVisitor::visitPost(DocHtmlHeader *node)
{
    printf("visitPost(DocHtmlHeader)\n");
}

void PickleDocVisitor::visitPre(DocImage *node)
{
    printf("visitPre(DocImage)\n");

}

void PickleDocVisitor::visitPost(DocImage *node)
{
    printf("visitPost(DocImage)\n");
}

void PickleDocVisitor::visitPre(DocDotFile *node)
{
    printf("visitPre(DocDotFile)\n");

}

void PickleDocVisitor::visitPost(DocDotFile *node)
{
    printf("visitPost(DocDotFile)\n");
}


void PickleDocVisitor::visitPre(DocMscFile *node)
{
    printf("visitPre(DocMscFile)\n");

}

void PickleDocVisitor::visitPost(DocMscFile *node)
{
    printf("visitPost(DocMscFile)\n");
}

void PickleDocVisitor::visitPre(DocDiaFile *node)
{
    printf("visitPre(DocDiaFile)\n");

}

void PickleDocVisitor::visitPost(DocDiaFile *node)
{
    printf("visitPost(DocDiaFile)\n");
}

void PickleDocVisitor::visitPre(DocLink *node)
{
    printf("visitPre(DocLink)\n");

}

void PickleDocVisitor::visitPost(DocLink *node)
{
    printf("visitPost(DocLink)\n");
}

void PickleDocVisitor::visitPre(DocRef *node)
{
    printf("visitPre(DocRef)\n");

}

void PickleDocVisitor::visitPost(DocRef *node)
{
    printf("visitPost(DocRef)\n");
}

void PickleDocVisitor::visitPre(DocSecRefItem *node)
{
    printf("visitPre(DocSecRefItem)\n");

}

void PickleDocVisitor::visitPost(DocSecRefItem *node)
{
    printf("visitPost(DocSecRefItem)\n");
}

void PickleDocVisitor::visitPre(DocSecRefList *node)
{
    printf("visitPre(DocSecRefList)\n");

}

void PickleDocVisitor::visitPost(DocSecRefList *node)
{
    printf("visitPost(DocSecRefList)\n");
}

void PickleDocVisitor::visitPre(DocXRefItem *node)
{
    printf("visitPre(DocXRefItem)\n");

}

void PickleDocVisitor::visitPost(DocXRefItem *node)
{
    printf("visitPost(DocXRefItem)\n");
}

void PickleDocVisitor::visitPre(DocInternalRef *node)
{
    printf("visitPre(DocInternalRef)\n");

}

void PickleDocVisitor::visitPost(DocInternalRef *node)
{
    printf("visitPost(DocInternalRef)\n");
}

void PickleDocVisitor::visitPre(DocCopy *node)
{
    printf("visitPre(DocCopy)\n");

}

void PickleDocVisitor::visitPost(DocCopy *node)
{
    printf("visitPost(DocCopy)\n");
}

void PickleDocVisitor::visitPre(DocHtmlBlockQuote *node)
{
    printf("visitPre(DocHtmlBlockQuote)\n");

}

void PickleDocVisitor::visitPost(DocHtmlBlockQuote *node)
{
    printf("visitPost(DocHtmlBlockQuote)\n");
}

void PickleDocVisitor::visitPre(DocVhdlFlow *node)
{
    printf("visitPre(DocVhdlFlow)\n");

}

void PickleDocVisitor::visitPost(DocVhdlFlow *node)
{
    printf("visitPost(DocVhdlFlow)\n");
}
