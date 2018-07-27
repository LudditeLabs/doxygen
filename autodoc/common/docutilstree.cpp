#include "autodoc/common/docutilstree.h"

namespace autodoc {

DocutilsTree::DocutilsTree()
: m_globals(PyGlobals::instance()),
  m_valid(false)
{

}
//-----------------------------------------------------------------------------

DocutilsTree::~DocutilsTree()
{

}
//-----------------------------------------------------------------------------

PyObject* DocutilsTree::create(const QCString &name,
                               PyObject *args,
                               PyObject *kwargs)
{
    PyObject *node = NULL;
    PyObjectPtr cls = m_globals->nodeClass(name);

    if (args != NULL && kwargs != NULL)
        node = PyObject_Call(cls, args, kwargs);

    // kwargs is NULL and args may be NULL.
    else
        node = PyObject_CallObject(cls, args);

    if (!node)
        printPyError();
    return node;
}
//-----------------------------------------------------------------------------

void DocutilsTree::pushWithName(const QCString &name,
                                PyObject *node,
                                bool append)
{
    // TODO: push node type to list.
    if (node == NULL)
        return;

    if (append)
        addTo(m_current, node);

    m_current = node;
    m_nodeTypes.push_back(name);
}
//-----------------------------------------------------------------------------

void DocutilsTree::pop(const char *nodeType)
{
    if (m_current.get() != m_document.get())
    {
        if (nodeType && nodeType != currentType())
        {
            printf("POP: node type is different: %s (expected %s)\n",
                   currentType().data(), nodeType);
            return;
        }
        m_current = getParentOf(m_current);
        m_nodeTypes.pop_back();
    }
}
//-----------------------------------------------------------------------------

PyObject* DocutilsTree::createTextNode(const QCString &text)
{
    PyObjectPtr str = PyUnicode_DecodeUTF8(text.data(), text.size(), "replace");
    return createWithArgs("Text", str.get(), NULL);
}
//-----------------------------------------------------------------------------

bool DocutilsTree::addTo(PyObject *parent, PyObject *node)
{
    PyObjectPtr meth = PyUnicode_FromString("append");
    PyObjectPtr res = PyObject_CallMethodObjArgs(parent, meth, node, NULL);
    return res ? true : printPyError();
}
//-----------------------------------------------------------------------------

PyObject* DocutilsTree::getParentOf(PyObject *node)
{
    return PyObject_GetAttrString(node, "parent");
}
//-----------------------------------------------------------------------------

PyObject* DocutilsTree::createDocument(PyObject *kwargs)
{
    PyObjectPtr none = Py_None;
    none.incRef();  // increment for first arg
    none.incRef();  // increment for second arg
    PyObjectPtr args = Py_BuildValue("(OO)", none.get(), none.get());

    m_document = m_current = create("document", args.get(), kwargs);
    m_valid = !m_document.isNull();

    m_nodeTypes.push_back("document");

    // Don't decrement document reference since it assigned to another var.
    m_current.setBorrowed();
    return m_current;
}
//-----------------------------------------------------------------------------

Py_ssize_t DocutilsTree::len(PyObject *node) const
{
    Py_ssize_t sz = PySequence_Length(node);
    if (sz == -1)
        printPyError();
    return sz;
}
//-----------------------------------------------------------------------------

bool DocutilsTree::removeChild(PyObject *node, Py_ssize_t index)
{
    PyObjectPtr i = PyLong_FromSsize_t(index);
    if (PyMapping_DelItem(node, i) == -1)
        return printPyError();
    return true;
}
//-----------------------------------------------------------------------------

} // namespace autodoc
