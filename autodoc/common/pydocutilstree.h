#ifndef AUTODOC_COMMON_PYDOCUTILSTREE_H
#define AUTODOC_COMMON_PYDOCUTILSTREE_H

#include "Python.h"
#include <qstringlist.h>
#include <deque>
#include "autodoc/common/utils.h"
#include "autodoc/common/pyglobals.h"


// TODO: do nothing if isValid() = false.
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
        if (!node)
            printPyError();
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
    void pushWithName(const QCString &name, PyObject *node, bool append = true);

    void pop(PyObject *newParent = NULL);

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

    bool isValid() const { return m_valid && m_globals->isValid(); }

    PyObject* getParentOf(PyObject *node);
    PyObject* current() const { return m_current.get(); }
    PyObject* document() const { return m_document.get(); }
    PyObject* createDocument(PyObject *kwargs = NULL);

    PyObject* takeDocument() { return m_document.take(); }

    Py_ssize_t len(PyObject *node) const;
    bool removeChild(PyObject *node, Py_ssize_t index);

private:
    PyGlobals *m_globals;
    PyObjectPtr m_document;
    PyObjectPtr m_current;
    std::deque<QCString> m_nodeTypes;
    QCString m_empty;
    bool m_valid;
};

#endif // AUTODOC_COMMON_PYDOCUTILSTREE_H
