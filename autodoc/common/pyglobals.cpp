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
    {
        printf("ERROR: can't import docutils.nodes.\n");
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

    return true;
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
        PyErr_Clear();
        return false;
    }

    m_dump = PyObject_GetAttrString(pickle, "dump");
    if (!m_dump)
    {
        printf("ERROR: can't get io.open().\n");
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

    // Cache io.open

    PyObjectPtr io = PyImport_ImportModule("io");
    if (!io)
    {
        printf("ERROR: can't import io.\n");
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

    m_open = PyObject_GetAttrString(io, "open");
    if (!m_open)
    {
        printf("ERROR: can't get io.open().\n");
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

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
    {
        printf("ERROR: can't open file.\n");
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

    PyObjectPtr res = PyObject_CallFunctionObjArgs(m_dump, node, file.get(), NULL);
    if (!res) {
        printf("ERROR: can't dump.\n");
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

    Py_XDECREF(PyObject_CallMethod(file, "flush", NULL));
    Py_XDECREF(PyObject_CallMethod(file, "close", NULL));

    if (PyErr_Occurred())
    {
        PyErr_Print();
        PyErr_Clear();
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
