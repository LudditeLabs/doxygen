#include "autodoc/tests/utils.h"
#include "Python.h"
#include <memory>
#include "docparser.h"
#include "filedef.h"
#include "autodoc/common/pydocutilstree.h"
#include "autodoc/common/visitor.h"

// Partially based on:
// https://stackoverflow.com/questions/1796510/accessing-a-python-traceback-from-the-c-api

// Print python tracepack.
static void printTraceback(PyObject *type, PyObject *value, PyObject *traceback)
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
            std::cout << strVal;
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


::testing::AssertionResult testutils::getPyError(const char *prefix)
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
    const char *value =  PyUnicode_AsUTF8(valueStr.get());

    printTraceback(exType, exValue, traceback);

    // When using PyErr_Restore() there is no need to decrement refs
    // for these 3 pointers.
    PyErr_Restore(exType, exValue, traceback);

    PyErr_Clear();

    ::testing::AssertionResult ex = ::testing::AssertionFailure();
    if (prefix)
        ex << prefix << ": ";
    return ex << name << ": " << value;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
