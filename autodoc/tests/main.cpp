#include "Python.h"
#include <iostream>
#include <memory>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "doxygen.h"
#include "config.h"
#include "configimpl.h"
#include "searchindex.h"
#include "store.h"
#include "autodoc/common/utils.h"
#include "qfileinfo.h"
#include "testconfig.h"


class DoxygenEnv: public ::testing::Environment
{
public:
    void SetUp() override
    {
        m_pyInit.reset(new PyInitHelper);

        std::stringstream code;
        code << "import sys\n"
                "sys.path.append(\"" << TEST_PYTHON_MODULES_PATH << "\")\n"
                "sys.path.append(\"" << TEST_PYTHON_FILES_PATH << "\")\n"
                "sys.argv = ['test']\n";

        PyRun_SimpleString(code.str().data());

        initDoxygen();
        Config::init();
        checkConfiguration();
        adjustConfiguration();
    }

    void TearDown() override
    {
        // From generateOutput() - doxygen.cpp
        cleanUpDoxygen();
        finializeSearchIndexer();
        Config::deinit();
        delete Doxygen::symbolMap;
        delete Doxygen::clangUsrMap;
        delete Doxygen::symbolStorage;
    }

private:
    std::unique_ptr<PyInitHelper> m_pyInit;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleMock(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new DoxygenEnv);
  return RUN_ALL_TESTS();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
