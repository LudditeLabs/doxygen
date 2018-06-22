#include "autodoc/common/pyglobals.h"


PyGlobals *PyGlobals::m_instance = nullptr;


PyGlobals::PyGlobals()
: m_valid(false)
{
    if (!setupDocutils() || !setupPickle())
    {
        m_dump.release();
        m_open.release();
    }
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

    m_dump = PyObject_GetAttrString(pickle, "dump");
    if (!m_dump)
        return printPyError("can't get io.open().");

    // Cache io.open

    PyObjectPtr io = PyImport_ImportModule("io");
    if (!io)
        return printPyError("can't import io.");

    m_open = PyObject_GetAttrString(io, "open");
    if (!m_open)
        return printPyError("can't get io.open().");

    return true;
}
//-----------------------------------------------------------------------------

bool PyGlobals::pickleToFile(PyObject *node, const QCString &fileName)
{
    if (!isValid())
        return false;

    PyObjectPtr n = PyUnicode_FromString(fileName.data());
    PyObjectPtr wb = PyUnicode_FromString("wb");
    PyObjectPtr file = PyObject_CallFunctionObjArgs(m_open, n.get(), wb.get(), NULL);
    if (!file)
        return printPyError("ERROR: can't open file.");

    PyObjectPtr res = PyObject_CallFunctionObjArgs(m_dump, node, file.get(), NULL);
    if (!res)
        return printPyError("can't dump.");

    res = PyObject_CallMethod(file, "flush", NULL);
    if (!res)
        return printPyError("can't flush file.");

    res = PyObject_CallMethod(file, "close", NULL);
    if (!res)
        return printPyError("can't close file.");

    return true;
}
//-----------------------------------------------------------------------------
