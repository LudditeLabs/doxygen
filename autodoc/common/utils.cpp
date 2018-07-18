#include "Python.h"
#include "autodoc/common/utils.h"
#include <iostream>
#include <ctype.h>

// For debugging purpose.
#define TRACE_REF(txt) ((void)0)
//#define TRACE_REF(txt)  if (m_p) \
//    std::cerr << txt << " " << m_p->ob_type->tp_name << " " << m_p << std::endl


// Partially based on:
// https://stackoverflow.com/questions/1796510/accessing-a-python-traceback-from-the-c-api
void getTraceback(PyObject *type, PyObject *value, PyObject *traceback, QCString *out)
{
    PyObjectPtr traceback_mod = PyImport_ImportModule("traceback");
    if (!traceback)
        return;

    PyObjectPtr fmt = PyObject_GetAttrString(traceback_mod, "format_exception");
    if (!fmt)
        return;

    PyObjectPtr tb = PyObject_CallFunctionObjArgs(fmt, type, value, traceback,
                                                  NULL);

    if (tb)
    {
        PyObjectPtr iterator = PyObject_GetIter(tb);
        PyObjectPtr item;
        const char *strVal;
        while (item = PyIter_Next(iterator))
        {
            strVal =  PyUnicode_AsUTF8(item.get());
            out->append(strVal);
        }
    }
}
//-----------------------------------------------------------------------------

QCString getPyError(bool clearError)
{
    if (PyErr_Occurred())
    {
        PyObject *exType;
        PyObject *exValue;
        PyObject *traceback;

        PyErr_Fetch(&exType, &exValue, &traceback);
        PyErr_NormalizeException(&exType, &exValue, &traceback);

        // Exception class name.
        PyObjectPtr nameStr = PyObject_GetAttrString(exType, "__name__");
        const char *name =  PyUnicode_AsUTF8(nameStr.get());

        // Exception value.
        PyObjectPtr valueStr = PyObject_Str(exValue);
        const char *val = PyUnicode_AsUTF8(valueStr.get());

        QCString result = val;
        result.append(": ").append(name).append("\n");
        getTraceback(exType, exValue, traceback, &result);

        // When using PyErr_Restore() there is no need to decrement refs
        // for these 3 pointers.
        PyErr_Restore(exType, exValue, traceback);

        if (clearError)
            PyErr_Clear();

        return result;
    }

    return QCString();
}
//-----------------------------------------------------------------------------

bool printPyError(const char *message)
{
    if (message)
        std::cerr << "ERROR: " << message << std::endl;
    QCString val = getPyError();
    std::cout << val.data();
    return false;
}
//-----------------------------------------------------------------------------

bool checkPyError(const char *message)
{
    return PyErr_Occurred() ? printPyError(message) : true;
}
//-----------------------------------------------------------------------------

bool stripTrailing(QCString *text)
{
    const uint size = text->size();
    uint i = size - 1;

    while (i && (isspace(static_cast<uchar>(text->at(i))) || text->at(i) == '\n'))
        --i;

    if (i < size - 1)
    {
        ++i;
        text->remove(i, size - i);
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


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
    TRACE_REF("construct");
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
        TRACE_REF("Py_DECREF");
        Py_DECREF(m_p);
    }
}

void PyObjectPtr::reset(PyObject *other)
{
    m_p = other;
    m_needDecRef = true;
    TRACE_REF("reset to");
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
            return printPyError();
        }
    }
    return true;
}
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
        PyTuple_SetItem(m_object, m_index++, PyUnicode_FromStringAndSize(str.data(), str.size()));
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
        PyObjectPtr val = PyUnicode_FromStringAndSize(value.data(), value.size());
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
