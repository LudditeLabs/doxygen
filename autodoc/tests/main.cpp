#include "Python.h"
#include <iostream>
#include <memory>
#include <string>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "autodoc/tests/utils.h"


GTEST_API_ int main(int argc, char** argv)
{
    std::string pytest_expression;

    for (int i = 0; i < argc; ++i)
    {
        std::string opt = argv[i];
        if (opt == "--pytest-k")
        {
            if (i + 1 < argc)
                pytest_expression = argv[i + 1];
            else
            {
                std::cerr << "Mising value for --pytest-k"  << std::endl;
                return 1;
            }
        }
    }

    testing::InitGoogleMock(&argc, argv);
    ::testing::AddGlobalTestEnvironment(
                new testutils::TestingEnv(pytest_expression));
    return RUN_ALL_TESTS();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
