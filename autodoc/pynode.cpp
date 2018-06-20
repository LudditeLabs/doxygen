#include "autodoc/pynode.h"

PyGlobals *PyGlobals::m_instance = nullptr;


PyGlobals::PyGlobals()
{
    m_nodesMod = PyImport_ImportModule("docutils.nodes");

    if (!setupPickle())
    {
        m_dump.release();
        m_open.release();
    }
}
//-----------------------------------------------------------------------------

PyGlobals::~PyGlobals()
{

}
//-----------------------------------------------------------------------------

PyGlobals *PyGlobals::instance()
{
    if (!m_instance)
        m_instance = new PyGlobals;
    return m_instance;
}
//-----------------------------------------------------------------------------

bool PyGlobals::setupPickle()
{
    // Cache pickle.dump

    PyObjectPtr pickle = PyImport_ImportModule("pickle");
    if (!pickle)
    {
        printf("ERROR: can't import pickle.\n");
        PyErr_Print();
        return false;
    }

    m_dump = PyObject_GetAttrString(pickle, "dump");
    if (!m_dump)
    {
        printf("ERROR: can't get io.open().\n");
        PyErr_Print();
        return false;
    }

    // Cache io.open

    PyObjectPtr io = PyImport_ImportModule("io");
    if (!io)
    {
        printf("ERROR: can't import io.\n");
        PyErr_Print();
        return false;
    }

    m_open = PyObject_GetAttrString(io, "open");
    if (!m_open)
    {
        printf("ERROR: can't get io.open().\n");
        PyErr_Print();
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


PyNode *PyNode::m_instance = nullptr;


PyNode::PyNode()
: m_nodesMod(PyImport_ImportModule("docutils.nodes"))
{
    if (!setupPickle())
    {
        m_dump.release();
        m_open.release();
    }
}
//-----------------------------------------------------------------------------

PyNode::~PyNode()
{

}
//-----------------------------------------------------------------------------

bool PyNode::setupPickle()
{
    // Cache pickle.dump

    PyObjectPtr pickle = PyImport_ImportModule("pickle");
    if (!pickle)
    {
        printf("ERROR: can't import pickle.\n");
        PyErr_Print();
        return false;
    }

    m_dump = PyObject_GetAttrString(pickle, "dump");
    if (!m_dump)
    {
        printf("ERROR: can't get io.open().\n");
        PyErr_Print();
        return false;
    }

    // Cache io.open

    PyObjectPtr io = PyImport_ImportModule("io");
    if (!io)
    {
        printf("ERROR: can't import io.\n");
        PyErr_Print();
        return false;
    }

    m_open = PyObject_GetAttrString(io, "open");
    if (!m_open)
    {
        printf("ERROR: can't get io.open().\n");
        PyErr_Print();
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

PyNode* PyNode::instance()
{
    if (!m_instance)
    {
        m_instance = new PyNode;
    }
    return m_instance;
}
//-----------------------------------------------------------------------------

PyObject* PyNode::create(const char *name, PyObject *args, PyObject *kw)
{
    PyObject *node = NULL;
    PyObjectPtr cls = PyObject_GetAttrString(m_nodesMod, name);

    if (args != NULL && kw != NULL)
    {
        node = PyObject_Call(cls, args, kw);
    }

    // kw is NULL and args may be NULL.
    else
    {
        node = PyObject_CallObject(cls, args);
    }

    if (!node)
    {
        PyErr_Print();
    }

    return node;
}
//-----------------------------------------------------------------------------

PyObject* PyNode::createTextNode(const QCString &text)
{
    PyObjectPtr str = PyUnicode_FromString(text.data());
    return createWithArgs("Text", str.get(), NULL);
}
//-----------------------------------------------------------------------------

bool PyNode::appendTo(PyObject *parent, PyObject *node)
{
    PyObjectPtr meth = PyUnicode_FromString("append");
    PyObjectPtr res = PyObject_CallMethodObjArgs(parent, meth, node, NULL);
    if (!res)
    {
        PyErr_Print();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------

PyObject* PyNode::getParent(PyObject *node)
{
    return PyObject_GetAttrString(node, "parent");
}
//-----------------------------------------------------------------------------

bool PyNode::pickle(PyObject *node, const QCString &fileName)
{
    if (!m_dump)
        return false;

    PyObjectPtr n = PyUnicode_FromString(fileName.data());
    PyObjectPtr wb = PyUnicode_FromString("wb");
    PyObjectPtr file = PyObject_CallFunctionObjArgs(m_open, n.get(), wb.get(), NULL);
    if (!file)
    {
        printf("ERROR: can't open file.\n");
        PyErr_Print();
        return false;
    }

    PyObjectPtr res = PyObject_CallFunctionObjArgs(m_dump, node, file.get(), NULL);
    if (!res) {
        printf("ERROR: can't dump.\n");
        PyErr_Print();
        return 1;
    }

    Py_XDECREF(PyObject_CallMethod(file, "flush", NULL));
    Py_XDECREF(PyObject_CallMethod(file, "close", NULL));

    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    else
    {
        printf("ok.\n");
    }
    return 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


bool pickleToFile(PyObject *node, const QCString &fileName)
{
    PyGlobals *g = PyGlobals::instance();

    if (!g->dump())
        return false;

    PyObjectPtr n = PyUnicode_FromString(fileName.data());
    PyObjectPtr wb = PyUnicode_FromString("wb");
    PyObjectPtr file = PyObject_CallFunctionObjArgs(g->open(), n.get(), wb.get(), NULL);
    if (!file)
    {
        printf("ERROR: can't open file.\n");
        PyErr_Print();
        return false;
    }

    PyObjectPtr res = PyObject_CallFunctionObjArgs(g->dump(), node, file.get(), NULL);
    if (!res) {
        printf("ERROR: can't dump.\n");
        PyErr_Print();
        return 1;
    }

    Py_XDECREF(PyObject_CallMethod(file, "flush", NULL));
    Py_XDECREF(PyObject_CallMethod(file, "close", NULL));

    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    else
    {
        printf("ok.\n");
    }
    return 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


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
    {
        node = PyObject_Call(cls, args, kwargs);
    }

    // kwargs is NULL and args may be NULL.
    else
    {
        node = PyObject_CallObject(cls, args);
    }

    if (!node)
    {
        PyErr_Print();
    }

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
    if (!res)
    {
        PyErr_Print();
        return false;
    }
    return true;
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
//-----------------------------------------------------------------------------
