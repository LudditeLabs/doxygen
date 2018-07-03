#include <gtest/gtest.h>
#include "autodoc/common/utils.h"


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
