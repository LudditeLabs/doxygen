/* Copyright (C) 2018, Luddite Labs Inc.
 *
 * This file is part of doxygen.
 *
 * Doxygen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
