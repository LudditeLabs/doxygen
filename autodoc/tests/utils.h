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

#ifndef AUTODOC_TESTS_UTILS_H
#define AUTODOC_TESTS_UTILS_H

#include <qcstring.h>
#include <gtest/gtest.h>
#include <string.h>

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

namespace autodoc { class PyInitHelper; }

namespace testutils {
    /**
     * Extract python error and print traceback.
     * @param prefix Message to set before the actuall error message.
     * @returns Google Test assertion result.
     */
    ::testing::AssertionResult assertPyError(const char *prefix = nullptr);


    class TestingEnv: public ::testing::Environment
    {
    public:
        TestingEnv(const std::string& pytestExpression);

        static TestingEnv* instance();

        void SetUp() override;
        void TearDown() override;

        const std::string& pytestExpression() const
        {
            return m_pytestExpression;
        }

    private:
        std::unique_ptr<autodoc::PyInitHelper> m_pyInit;
        std::string m_pytestExpression;
    };
}

#endif // AUTODOC_TESTS_UTILS_H
