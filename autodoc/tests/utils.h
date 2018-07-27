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
