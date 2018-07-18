#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "autodoc/common/pydocutilstree.h"


void PyDocVisitor::visitPre(DocParamSect *node)
{
    TRACE_VISIT("visitPre(DocParamSect)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    m_paramType = node->type();
    if (!m_fieldList)
    {
        m_fieldList = m_tree->push("field_list");
        m_fieldList.setBorrowed();  // don't decrement in destructor.
    }
    else
    {
        // pushWithName() takes ownership, so we increment to prevent GC.
        // NOTE: this resets borrowed flag and we need to restore it later.
        m_fieldList.incRef();
        m_tree->pushWithName("field_list", m_fieldList, false);
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocParamSect *node)
{
    TRACE_VISIT("visitPost(DocParamSect)\n");
    if (!beforePost(node))
        return;
    
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);
    m_tree->pop();

    // Restore borrowed flag, see above.
    m_fieldList.setBorrowed();
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocParamList *node)
{
    TRACE_VISIT("visitPre(DocParamList)\n");
    if (!beforePre(node))
        return;

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

    PyObject *parent_of_fields = m_tree->current();

    m_tree->push("field", args.get(), kw.get());

    QCString rtype;

    PyDict fieldkw;  // optional names.
    if (!other_names.isNull())
        fieldkw.setField("names", other_names.get());

    // @param
    if(m_paramType == DocParamSect::Param)
    {
        name = "param " + name;
    }

    // @tparam
    else if (m_paramType == DocParamSect::TemplateParam)
    {
        name = "param " + name;
        Py_INCREF(Py_True);
        fieldkw.setField("is_template_param", Py_True);
        Py_DECREF(Py_True);
    }

    // @throw, @throws, @exception
    else if (m_paramType == DocParamSect::Exception)
    {
        // TODO: create raises field.
        name = "raises " + name;
    }

    else if (m_paramType == DocParamSect::RetVal)
    {
        // See bellow.
        rtype = name;
        name = "return";
    }

    else
    {
        // TODO: what to do?
        name = "N/A " + name;
    }

    PyObjectPtr text = m_tree->createTextNode(name);
    PyObjectPtr field_name = m_tree->create("field_name", args.get(), fieldkw.get());
    m_tree->addTo(field_name, text);
    m_tree->addToCurrent(field_name);

    // @retval
    // From http://doxygen.10944.n7.nabble.com/return-vs-retval-td2096.html:
    // \retval is meant to list the return values, not the type. So for
    // instance, if you have a function that
    // returns an error code, \retval can be used to list the error code values
    // that could be returned by the function and their meaning.
    //
    // For this node we add two fields: <return> and <rtype>
    if (m_paramType == DocParamSect::RetVal)
    {
        PyObjectPtr rtype_field = m_tree->create("field");

        PyObjectPtr text = m_tree->createTextNode("rtype");
        PyObjectPtr field_name = m_tree->create("field_name");
        m_tree->addTo(field_name, text);

        PyObjectPtr field_body = m_tree->create("field_body");
        text = m_tree->createTextNode(rtype);
        m_tree->addTo(field_body, text);

        m_tree->addTo(rtype_field, field_name);
        m_tree->addTo(rtype_field, field_body);

        m_tree->addTo(parent_of_fields, rtype_field);
    }

    m_tree->push("field_body");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocParamList *node)
{
    TRACE_VISIT("visitPost(DocParamList)\n");
    if (!beforePost(node))
        return;
    
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    // field_body -> field.
    m_tree->pop();
    // field -> field_list.
    m_tree->pop();
}
//-----------------------------------------------------------------------------

static QCString get_simple_sect_field_name(DocSimpleSect *node)
{
    switch (node->type())
    {
    case DocSimpleSect::Return:     return "return";
    case DocSimpleSect::Author:     return "Author";
    case DocSimpleSect::Authors:    return "Authors";
    case DocSimpleSect::Version:    return "Version";
    case DocSimpleSect::Date:       return "Date";
    case DocSimpleSect::Copyright:  return "copyright";
    default:
        return "unknown";
    }
}

void PyDocVisitor::visitPre(DocSimpleSect *node)
{
    TRACE_VISIT("visitPre(DocSimpleSect)\n");
    if (!beforePre(node))
        return;

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    QCString name;

    switch(node->type())
    {
    // Fields.
    case DocSimpleSect::Return:
    case DocSimpleSect::Author:
    case DocSimpleSect::Authors:
    case DocSimpleSect::Version:
    case DocSimpleSect::Date:
    case DocSimpleSect::Copyright:
        {
            m_tree->push("field");
            PyObjectPtr text = m_tree->createTextNode(get_simple_sect_field_name(node));
            PyObjectPtr field_name = m_tree->create("field_name");
            m_tree->addTo(field_name, text);
            m_tree->addToCurrent(field_name);
            m_tree->push("field_body");
        }
        break;

    // RCS (Revision Control System, CSV, perforce) keyword: $<ID>:<text>$
    // Example: $Revision: #19 $
    //          $Author:Chuck$
    case DocSimpleSect::Rcs:
    {
        PyObjectPtr args = PyTuple_New(0);
        PyDict kw;
        kw.setField("rcs", "1");
        // field_name is created in visitPre(DocTitle).
        // field_body is created in visitPost(DocTitle).
        m_tree->push("field", args, kw);
        break;
    }

    // Directives.
    // @since {text or paragraph}
    case DocSimpleSect::Since:
    {
        PyObjectPtr args = PyTuple_New(0);
        PyDict kw;
        kw.setField("type", "versionadded");
        m_tree->push("admonition", args, kw);
        break;
    }
    case DocSimpleSect::See:
        name = "see";
        break;
    case DocSimpleSect::Note:
        name = "note";
        break;
    case DocSimpleSect::Warning:
        name = "warning";
        break;
    case DocSimpleSect::Attention:
        name = "attention";
        break;

    // \pre -  Starts a paragraph where the precondition of an entity
    //         can be described.
    case DocSimpleSect::Pre:
        name = "pre";
        break;
    // \post - Starts a paragraph where the postcondition of an entity
    //         can be described.
    case DocSimpleSect::Post:
        name = "post";
        break;

    // \invariant - Starts a paragraph where the invariant of an entity
    //              can be described.
    case DocSimpleSect::Invar:
        name = "invariant";
        break;

    // \remark, \remarks - Starts a paragraph where one or more remarks
    //                     may be entered.
    case DocSimpleSect::Remark:
        name = "remark";
        break;

    case DocSimpleSect::User:
        name = "par";
        break;

    case DocSimpleSect::Unknown:
        break;
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocSimpleSect *node)
{
    TRACE_VISIT("visitPost(DocSimpleSect)\n");
    if (!beforePost(node))
        return;
    
    if (node->type() == DocSimpleSect::Since)
    {
        // TODO: check for errors.
        // See: http://www.sphinx-doc.org/en/stable/markup/para.html#directive-versionadded
        // Extract first line from the first paragraph and put it to
        // admonition's attr 'version'.
        PyObject *since = m_tree->current();
        if (PySequence_Length(since))
        {
            PyObjectPtr par = PySequence_GetItem(since, 0);
            if (PySequence_Length(par))
            {
                PyObjectPtr text = PySequence_GetItem(par, 0);
                if (PySequence_Length(text))
                {
                    PyObjectPtr meth = PyUnicode_FromString("astext");
                    PyObjectPtr str = PyObject_CallMethodObjArgs(text, meth, NULL);
                    QCString t = PyUnicode_AsUTF8(str);
                    int i = t.find('\n');
                    QCString left;
                    QCString right;
                    if (i != -1)
                    {
                        left = t.left(i);
                        right = t.right(t.length() - i - 1);
                    }
                    else
                        left = t;

                    PyObjectPtr stro = PyUnicode_DecodeUTF8(left.data(), left.length(), "replace");
                    PyMapping_SetItemString(since, "version", stro);

                    // Remove nested paragrahp if remaining text is empty.
                    if (right.isEmpty())
                    {
                        PyObjectPtr index = PyLong_FromLong(0);
                        PyObject_DelItem(since, index);
                    }
                    // Replace text in paragraph.
                    else
                    {
                        PyObjectPtr index = PyLong_FromLong(0);
                        PyObjectPtr newt = m_tree->createTextNode(right.data());
                        PyObject_SetItem(par, index, newt);
                    }
                }
            }
        }
    }

    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    switch(node->type())
    {
    // Fields.
    case DocSimpleSect::Return:
    case DocSimpleSect::Author:
    case DocSimpleSect::Authors:
    case DocSimpleSect::Version:
    case DocSimpleSect::Date:
    case DocSimpleSect::Copyright:
    case DocSimpleSect::Rcs:
        m_tree->pop();  // field_body -> field
        m_tree->pop();  // field -> field_list
        break;
    default:
        m_tree->pop();
        break;
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPre(DocTitle *node)
{
    TRACE_VISIT("visitPre(DocTitle)\n");
    if (!beforePre(node))
        return;

    // See visitPre(DocSimpleSect)
    m_tree->push("field_name");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visitPost(DocTitle *node)
{
    TRACE_VISIT("visitPost(DocTitle)\n");
    if (!beforePost(node))
        return;

    maybeCreateTextNode();      // create text node from DocWord.
    m_tree->pop();              // field_name -> field
    m_tree->push("field_body"); // field -> field_body
}
//-----------------------------------------------------------------------------
