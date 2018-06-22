#include "autodoc/common/pydocutilstree.h"


PyDocutilsTree::PyDocutilsTree()
: m_globals(PyGlobals::instance()),
  m_valid(false)
{

}
//-----------------------------------------------------------------------------

PyDocutilsTree::~PyDocutilsTree()
{

}
//-----------------------------------------------------------------------------

PyObject* PyDocutilsTree::create(const QCString &name,
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

void PyDocutilsTree::pushWithName(const QCString &name, PyObject *node,
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

void PyDocutilsTree::pop(PyObject *newParent)
{
    if (m_current.get() != m_document.get())
    {
        if (newParent)
            m_current = newParent;
        else
            m_current = getParentOf(m_current);
        m_nodeTypes.pop_back();
    }
}
//-----------------------------------------------------------------------------

PyObject* PyDocutilsTree::createTextNode(const QCString &text)
{
    PyObjectPtr str = PyUnicode_FromString(text.data());
    return createWithArgs("Text", str.get(), NULL);
}
//-----------------------------------------------------------------------------

bool PyDocutilsTree::addTo(PyObject *parent, PyObject *node)
{
    PyObjectPtr meth = PyUnicode_FromString("append");
    PyObjectPtr res = PyObject_CallMethodObjArgs(parent, meth, node, NULL);
    return res ? true : printPyError();
}
//-----------------------------------------------------------------------------

PyObject* PyDocutilsTree::getParentOf(PyObject *node)
{
    return PyObject_GetAttrString(node, "parent");
}
//-----------------------------------------------------------------------------

PyObject* PyDocutilsTree::createDocument(PyObject *kwargs)
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
