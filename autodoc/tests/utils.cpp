#include "Python.h"
#include "autodoc/tests/utils.h"
#include "autodoc/common/docutilstree.h"
#include "autodoc/common/visitor.h"
#include "autodoc/common/utils.h"
#include "testconfig.h"
#include "docparser.h"
#include "filedef.h"
#include "doxygen.h"
#include "config.h"
#include "configimpl.h"
#include "searchindex.h"
#include "store.h"
#include "qfileinfo.h"
#include <gtest/gtest.h>
#include <memory>
#include <iostream>


::std::ostream& operator<<(::std::ostream& os, const QCString& val)
{
    os << "[" << val.data() << "]";
    return os;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


::testing::AssertionResult testutils::assertPyError(const char *prefix)
{
    QCString err = autodoc::getPyError();
    ::testing::AssertionResult ex = ::testing::AssertionFailure();
    if (prefix)
        ex << prefix << ": ";
    return ex << err.data();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


static testutils::TestingEnv *testEnv = nullptr;

testutils::TestingEnv::TestingEnv(const std::string &pytestExpression)
    : m_pytestExpression(pytestExpression)
{
    testEnv = this;
}
//-----------------------------------------------------------------------------

testutils::TestingEnv* testutils::TestingEnv::instance()
{
    return testEnv;
}
//-----------------------------------------------------------------------------

void testutils::TestingEnv::SetUp()
{
    m_pyInit.reset(new autodoc::PyInitHelper);

    std::stringstream code;
    code << "import sys\n"
         << "sys.path.append(\"" << TEST_PYTHON_MODULES_PATH << "\")\n"
         << "sys.path.append(\"" << TEST_PYTHON_FILES_PATH << "\")\n"
         << "sys.argv = ['test']\n";

    PyRun_SimpleString(code.str().data());

    initDoxygen();
    Config::init();
    checkConfiguration();
    adjustConfiguration();
}
//-----------------------------------------------------------------------------

void testutils::TestingEnv::TearDown()
{
    // From generateOutput() - doxygen.cpp
    cleanUpDoxygen();
    finializeSearchIndexer();
    Config::deinit();
    delete Doxygen::symbolMap;
    delete Doxygen::clangUsrMap;
    delete Doxygen::symbolStorage;
}
//-----------------------------------------------------------------------------
