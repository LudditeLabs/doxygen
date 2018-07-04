#include "autodoc/common/pyglobals.h"


PyGlobals *PyGlobals::m_instance = nullptr;


PyGlobals::PyGlobals()
: m_valid(false)
{
    if (!setupDocutils() || !setupPickle())
        m_dumps.release();
    else
        m_valid = true;
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

bool PyGlobals::setupDocutils()
{
    m_nodesMod = PyImport_ImportModule("docutils.nodes");
    if (!m_nodesMod)
        return printPyError("can't import docutils.nodes.");
    return true;
}
//-----------------------------------------------------------------------------

bool PyGlobals::setupPickle()
{
    // Cache pickle.dump

    PyObjectPtr pickle = PyImport_ImportModule("pickle");
    if (!pickle)
        return printPyError("can't import pickle.");

    m_dumps = PyObject_GetAttrString(pickle, "dumps");
    if (!m_dumps)
        return printPyError("can't get pickle.dumps().");

    // Cache io.open

    PyObjectPtr io = PyImport_ImportModule("io");
    if (!io)
        return printPyError("can't import io.");

    return true;
}
//-----------------------------------------------------------------------------

PyObject* PyGlobals::pickleToString(PyObject *object)
{
    if (!isValid())
        return NULL;

    PyObject* res = PyObject_CallFunctionObjArgs(m_dumps, object, NULL);
    if (!res)
        printPyError("can't call pickle.dumps().");

    return res;
}
//-----------------------------------------------------------------------------
