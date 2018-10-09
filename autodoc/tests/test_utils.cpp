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

#include <gtest/gtest.h>
#include "autodoc/common/utils.h"

using namespace autodoc;


TEST(TestUtilsStrip, nospaces)
{
    QCString text = "hello";
    stripTrailing(&text);
    ASSERT_EQ("hello", text);
}
//-----------------------------------------------------------------------------

TEST(TestUtilsStrip, two_words)
{
    QCString text = "hello hello";
    stripTrailing(&text);
    ASSERT_EQ("hello hello", text);
}
//-----------------------------------------------------------------------------

TEST(TestUtilsStrip, one_space)
{
    QCString text = "hello ";
    stripTrailing(&text);
    ASSERT_EQ("hello", text);
}
//-----------------------------------------------------------------------------

TEST(TestUtilsStrip, multi_space)
{
    QCString text = "hello   ";
    stripTrailing(&text);
    ASSERT_EQ("hello", text);
}
//-----------------------------------------------------------------------------

TEST(TestUtilsStrip, line_break)
{
    QCString text = "hello\n";
    stripTrailing(&text);
    ASSERT_EQ("hello", text);
}
//-----------------------------------------------------------------------------

TEST(TestUtilsStrip, line_break_with_spaces)
{
    QCString text = "hello \n   \n";
    stripTrailing(&text);
    ASSERT_EQ("hello", text);
}
//-----------------------------------------------------------------------------
