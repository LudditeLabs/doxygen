#include <gtest/gtest.h>
#include "Python.h"
#include "autodoc/common/docutilstree.h"
#include "testconfig.h"
#include "autodoc/common/utils.h"
#include "autodoc/tests/utils.h"

using namespace testutils;


static ::testing::AssertionResult do_chdir(const char *path)
{
    PyObjectPtr os = PyImport_ImportModule("os");
    if (!os)
        return assertPyError();

    PyObjectPtr chdir = PyObject_GetAttrString(os, "chdir");
    if (!chdir)
        return assertPyError();

    PyObjectPtr val = PyUnicode_FromString(path);

    PyObjectPtr res = PyObject_CallFunctionObjArgs(chdir, val.get(), NULL);
    if (!res)
        return assertPyError();

    return ::testing::AssertionSuccess();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


// NOTE: make sure pytest is installed:
//       wget https://bootstrap.pypa.io/get-pip.py
//       sudo python3.5dm get-pip.py
//       sudo python3.5dm -m pip install pytest
//
// NOTE: see also main.cpp where extra setup performs.
//
// 'expression' can be used to run tests by keyword expressions:
// "MyClass and not method"
static ::testing::AssertionResult runPyTest(const char *path)
{
    PyObjectPtr pytest = PyImport_ImportModule("pytest");
    if (!pytest)
        return assertPyError();

    PyObjectPtr main = PyObject_GetAttrString(pytest, "main");
    if (!main)
        return assertPyError();

    PyObjectPtr args = Py_BuildValue("[s]", path);

    auto expression = TestingEnv::instance()->pytestExpression();
    if (!expression.empty())
        args = Py_BuildValue("[ssss]", path, "-k", expression.data());
    else
        args = Py_BuildValue("[s]", path);

    PyObjectPtr res = PyObject_CallFunctionObjArgs(main, args.get(), NULL);
    if (!res)
        return assertPyError();

    long status = PyLong_AsLong(res);

    // https://docs.pytest.org/en/latest/usage.html
    if (status)
    {
        ::testing::AssertionResult ex = ::testing::AssertionFailure();

        switch (status) {
        case 1:
            ex << "Finished with errors";
            break;
        case 2:
            ex << "Test execution was interrupted by the user";
            break;
        case 3:
            ex << "Internal error happened while executing tests";
            break;
        case 4:
            ex << "pytest command line usage error";
            break;
        case 5:
            ex << "No tests were collected";
            break;
        default:
            ex << "Unknown reason: " << status;
            break;
        }
        return ex;
    }

    return ::testing::AssertionSuccess();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


TEST(TestPy, run)
{
    ASSERT_TRUE(do_chdir(TEST_PYTHON_FILES_PATH));
    ASSERT_TRUE(runPyTest(TEST_PYTHON_FILES_PATH));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
