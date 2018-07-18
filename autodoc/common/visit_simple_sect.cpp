#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "autodoc/common/docutilstree.h"


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
    case DocSimpleSect::Pre:        return "pre";
    case DocSimpleSect::Post:       return "post";
    case DocSimpleSect::Invar:      return "invariant";
    case DocSimpleSect::Remark:     return "remark";
    default:
        return "unknown";
    }
}

void DocutilsVisitor::visitPre(DocSimpleSect *node)
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
        m_titleParent = DocSimpleSect::Rcs;
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
    {
        PyObjectPtr args = PyTuple_New(0);
        PyDict kw;
        kw.setField("type", "seealso");
        m_tree->push("admonition", args, kw);
        break;
    }
    case DocSimpleSect::Note:
        m_tree->push("note");
        break;
    case DocSimpleSect::Warning:
        m_tree->push("warning");
        break;
    case DocSimpleSect::Attention:
    {
        PyObjectPtr args = PyTuple_New(0);
        PyDict kw;
        kw.setField("type", "attention");
        m_tree->push("admonition", args, kw);
        break;
    }

    // \pre -  Starts a paragraph where the precondition of an entity
    //         can be described.
    // \post - Starts a paragraph where the postcondition of an entity
    //         can be described.
    // \invariant - Starts a paragraph where the invariant of an entity
    //              can be described.
    // \remark, \remarks - Starts a paragraph where one or more remarks
    //                     may be entered.
    case DocSimpleSect::Pre:
    case DocSimpleSect::Post:
    case DocSimpleSect::Invar:
    case DocSimpleSect::Remark:
        break;  // see visitPost().

    case DocSimpleSect::User:
        m_titleParent = DocSimpleSect::User;
        break;  // see visitPost().

    case DocSimpleSect::Unknown:
        break;  // TODO: what is it?
    }
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocSimpleSect *node)
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
    case DocSimpleSect::Pre:
    case DocSimpleSect::Post:
    case DocSimpleSect::Invar:
    case DocSimpleSect::Remark:
    {
        PyObject *current = m_tree->current();
        PyObjectPtr index = PyLong_FromLong(-1);
        PyObjectPtr par = PyObject_GetItem(current, index);
        QCString str = get_simple_sect_field_name(node);
        PyObjectPtr key = PyUnicode_FromString("type");
        PyObjectPtr val = PyUnicode_FromStringAndSize(str.data(), str.size());
        PyObject_SetItem(par, key, val);
        break;
    }
    case DocSimpleSect::User:
    {
        // TODO: add error checking.
        // Move <inline> at -2 to the beginning of the paragraph at -1.
        PyObject *current = m_tree->current();

        // Take <inline> node.
        PyObjectPtr index = PyLong_FromLong(-2);
        PyObjectPtr meth = PyUnicode_FromString("pop");
        PyObjectPtr title = PyObject_CallMethodObjArgs(current, meth, index.get(), NULL);

        // Get paragraph after the <inline>.
        index = PyLong_FromLong(-1);
        PyObjectPtr par = PyObject_GetItem(current, index);
        meth = PyUnicode_FromString("insert");

        index = PyLong_FromLong(0);
        PyObjectPtr res = PyObject_CallMethodObjArgs(par, meth, index.get(),
                                                     title.get(), NULL);
        break;
    }

    case DocSimpleSect::Unknown:
        break;

    default:
        m_tree->pop();
        break;
    }
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPre(DocTitle *node)
{
    TRACE_VISIT("visitPre(DocTitle)\n");
    if (!beforePre(node))
        return;

    // See visitPre(DocSimpleSect)
    if (m_titleParent == DocSimpleSect::Rcs)
        m_tree->push("field_name");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visitPost(DocTitle *node)
{
    TRACE_VISIT("visitPost(DocTitle)\n");
    if (!beforePost(node))
        return;

    if (m_titleParent == DocSimpleSect::Rcs)
    {
        maybeCreateTextNode();      // create text node from DocWord.
        m_tree->pop();              // field_name -> field
        m_tree->push("field_body"); // field -> field_body
    }

    // Add temporary <inline> node with title.
    // It will be moved to the beginning of the paragraph.
    // See visitPost(DocSimpleSect) for DocSimpleSect::User.
    else if (m_titleParent == DocSimpleSect::User)
    {
        // NOTE: m_textBuf non empty for this node type (paragraph with title).
        ::stripTrailing(&m_textBuf);

        PyObjectPtr str = PyUnicode_DecodeUTF8(m_textBuf.data(),
                                               m_textBuf.size(), "replace");

        PyObjectPtr args = PyTuple_New(0);
        PyDict kw;
        kw.setField("rawsource", str);
        kw.setField("text", str);
        kw.setField("type", "title");
        PyObjectPtr text = m_tree->create("inline", args, kw);
        m_tree->addToCurrent(text);
        m_textBuf = QCString();
    }

    m_titleParent = -1;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocSimpleSectSep *node)
{
    // Separator between two simple sections of the same type.
    // Nothing to add to docutils tree.
    TRACE_VISIT("visit(DocSimpleSectSep)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------
