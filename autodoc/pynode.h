#ifndef PYNODE_H
#define PYNODE_H

#include "Python.h"
#include "autodoc/utils.h"
#include <qstringlist.h>
#include <deque>

/**
 * This class caches some python functions and modules.
 */
class PyGlobals
{
public:
    ~PyGlobals();

    static PyGlobals* instance();

    PyObject* nodesModule() const { return m_nodesMod.get(); }
    PyObject* dump() const { return m_dump.get(); }
    PyObject* open() const { return m_open.get(); }

    PyObject* nodeClass(const QCString &name) const
    {
        return PyObject_GetAttrString(m_nodesMod, name.data());
    }

private:
    static PyGlobals *m_instance;
    PyObjectPtr m_nodesMod;
    PyObjectPtr m_dump;
    PyObjectPtr m_open;

    PyGlobals();

    bool setupPickle();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/**
 * Helper class to operate with python docutils.nodes.
 */
class PyNode
{
public:
    PyNode();
    ~PyNode();

    static PyNode* instance();

    PyObject* create(const char *name,
                     PyObject *args = NULL,
                     PyObject *kw = NULL);

    template<typename ...Args>
    PyObject* createWithArgs(const char *name, Args ...args)
    {
        PyObjectPtr cls = PyObject_GetAttrString(m_nodesMod, name);
        PyObject *node = PyObject_CallFunctionObjArgs(cls, args...);

        if (node == NULL)
        {
            PyErr_Print();
        }
        return node;

    }

    PyObject* createTextNode(const QCString &text);

    // See docutils.nodes.Node class
    bool appendTo(PyObject *parent, PyObject *node);
    PyObject* getParent(PyObject *node);

    bool pickle(PyObject *node, const QCString &fileName);

private:
    static PyNode *m_instance;
    PyObjectPtr m_nodesMod;
    PyObjectPtr m_dump;
    PyObjectPtr m_open;

    bool setupPickle();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


bool pickleToFile(PyObject *node, const QCString &fileName);


class PyDocutilsTree
{
public:
    PyDocutilsTree();
    ~PyDocutilsTree();

    PyObject* create(const QCString &name,
                     PyObject *args = NULL,
                     PyObject *kwargs = NULL);

    template<typename ...Args>
    PyObject* createWithArgs(const QCString &name, Args ...args)
    {
        PyObjectPtr cls = m_globals->nodeClass(name);
        PyObject *node = PyObject_CallFunctionObjArgs(cls, args...);

        if (node == NULL)
            PyErr_Print();
        return node;
    }

    // NOTE: returns borrowed reference.
    PyObject* push(const QCString &name,
                   PyObject *args = NULL,
                   PyObject *kwargs = NULL)
    {
        PyObject *node = create(name, args, kwargs);
        pushWithName(name, node);
        return node;
    }

    // NOTE: returns borrowed reference.
    template<typename ...Args>
    PyObject* pushWithArgs(const QCString &name, Args ...args)
    {
        PyObject *node = createWithArgs(name, args...);
        pushWithName(name, node);
        return node;
    }

    // NOTE: takes ownership!
    void pushWithName(const QCString &name, PyObject *node);

    void pop();

    PyObject* createTextNode(const QCString &text);

    // See docutils.nodes.Node class
    bool addTo(PyObject *parent, PyObject *node);
    bool addToCurrent(PyObject *node) { return addTo(m_current, node); }

    const QCString& currentType() const
    {
        return m_nodeTypes.empty() ? m_empty : m_nodeTypes.back();
    }

    const QCString& parentType() const
    {
        int sz = m_nodeTypes.size() - 2;
        return sz < 0 ? m_empty : m_nodeTypes.at(sz);
    }

    bool isValid() const { return m_valid; }
    PyObject* getParentOf(PyObject *node);
    PyObject* current() const { return m_current.get(); }
    PyObject* document() const { return m_document.get(); }
    PyObject* createDocument(PyObject *kwargs = NULL);

private:
    PyGlobals *m_globals;
    PyObjectPtr m_document;
    PyObjectPtr m_current;
    std::deque<QCString> m_nodeTypes;
    QCString m_empty;
    bool m_valid;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif // PYNODE_H
