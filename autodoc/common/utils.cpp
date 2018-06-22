#include "Python.h"
#include "autodoc/common/utils.h"
#include <cstdarg>


PyInitHelper::PyInitHelper()
{
    Py_Initialize();
}
//-----------------------------------------------------------------------------

PyInitHelper::~PyInitHelper()
{
    Py_Finalize();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


PyObjectPtr::PyObjectPtr(PyObject *p)
: m_p(p), m_needDecRef(true)
{
//    if (m_p)
//        printf("construct %s %p\n", m_p->ob_type->tp_name, m_p);
}
//-----------------------------------------------------------------------------

PyObjectPtr::~PyObjectPtr()
{
    decRef();
}
//-----------------------------------------------------------------------------

void PyObjectPtr::incRef()
{
    if (m_p)
    {
        m_needDecRef = true;
        Py_INCREF(m_p);
    }
}
//-----------------------------------------------------------------------------

void PyObjectPtr::decRef()
{
    if (m_needDecRef && m_p)
    {
//        printf("Py_DECREF %s %p\n", m_p->ob_type->tp_name, m_p);
        Py_DECREF(m_p);
    }
}

void PyObjectPtr::reset(PyObject *other)
{
    m_p = other;
    m_needDecRef = true;

//    if (m_p)
//        printf("reset to %s %p\n", m_p->ob_type->tp_name, m_p);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


PyClass::PyClass(PyObject *object)
: m_createError(false), m_object(object)
{

}
//-----------------------------------------------------------------------------

PyClass::~PyClass()
{

}
//-----------------------------------------------------------------------------

bool PyClass::ensureCreated()
{
    if (!m_createError && !m_object)
    {
        m_object = create();
        if (!m_object)
        {
            m_createError = true;
            PyErr_Print();
            PyErr_Clear();
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


PyTuple::PyTuple(int size)
: PyClass(), m_size(size), m_index(0)
{

}
//-----------------------------------------------------------------------------

PyObject* PyTuple::create()
{
    return PyTuple_New(m_size);
}
//-----------------------------------------------------------------------------

void PyTuple::add(const QCString &str)
{
    if (ensureCreated())
        PyTuple_SetItem(m_object, m_index++, PyUnicode_FromString(str.data()));
}
//-----------------------------------------------------------------------------

void PyTuple::add(PyObject *object)
{
    if (ensureCreated())
        PyTuple_SetItem(m_object, m_index++, object);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


PyDict::PyDict(PyObject *object)
: PyClass(object)
{

}
//-----------------------------------------------------------------------------

PyObject* PyDict::create()
{
    return PyDict_New();
}
//-----------------------------------------------------------------------------

void PyDict::setField(const QCString &field, const QCString &value)
{
    if (ensureCreated())
    {
        PyObjectPtr val = PyUnicode_FromString(value.data());
        PyDict_SetItemString(m_object, field.data(), val.get());
    }
}
//-----------------------------------------------------------------------------

void PyDict::setField(const QCString &field, PyObject *object)
{
    if (ensureCreated())
        PyDict_SetItemString(m_object, field.data(), object);
}
//-----------------------------------------------------------------------------

void PyDict::setField(const QCString &field, const PyObjectPtr &ptr)
{
    if (ensureCreated())
        PyDict_SetItemString(m_object, field.data(), ptr.get());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
