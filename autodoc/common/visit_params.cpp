#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "autodoc/common/docutilstree.h"

namespace autodoc {

void DocutilsVisitor::visitPre(DocParamSect *node)
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

void DocutilsVisitor::visitPost(DocParamSect *node)
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

void DocutilsVisitor::visitPre(DocParamList *node)
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

void DocutilsVisitor::visitPost(DocParamList *node)
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

} // namespace autodoc
