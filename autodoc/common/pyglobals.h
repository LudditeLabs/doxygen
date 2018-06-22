#ifndef AUTODOC_COMMON_PYGLOBALS_H
#define AUTODOC_COMMON_PYGLOBALS_H

#include "Python.h"
#include <qstringlist.h>
#include <deque>
#include "autodoc/common/utils.h"


/**
 * This class provides access to some python functions and modules.
 */
class PyGlobals
{
public:
    ~PyGlobals();

    static PyGlobals* instance();

    PyObject* nodesModule() const { return m_nodesMod.get(); }
    PyObject* dump() const { return m_dump.get(); }
    PyObject* open() const { return m_open.get(); }

    /**
     * Get node class from <tt>docutils.nodes</tt>.
     *
     * @param Node class name.
     * @return Class object.
     */
    PyObject* nodeClass(const QCString &name) const
    {
        return PyObject_GetAttrString(m_nodesMod, name.data());
    }

    bool isValid() const { return m_valid; }

    bool pickleToFile(PyObject *node, const QCString &fileName);

private:
    static PyGlobals *m_instance;
    PyObjectPtr m_nodesMod;
    PyObjectPtr m_dump;
    PyObjectPtr m_open;
    bool m_valid;

    PyGlobals();

    bool setupDocutils();
    bool setupPickle();
};


#endif // AUTODOC_COMMON_PYGLOBALS_H
