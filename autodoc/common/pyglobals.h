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

    bool isValid() const { return m_valid; }

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

    PyObject* pickleToString(PyObject *object);

private:
    static PyGlobals *m_instance;
    PyObjectPtr m_nodesMod;
    PyObjectPtr m_dumps;
    bool m_valid;

    PyGlobals();

    bool setupDocutils();
    bool setupPickle();
};


#endif // AUTODOC_COMMON_PYGLOBALS_H
