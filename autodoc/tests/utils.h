#ifndef AUTODOC_TESTS_UTILS_H
#define AUTODOC_TESTS_UTILS_H

#include <qcstring.h>
#include <gtest/gtest.h>

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

namespace testutils {
    /**
     * Extract python error and print traceback.
     * @param prefix Message to set before the actuall error message.
     * @returns Google Test assertion result.
     */
    ::testing::AssertionResult getPyError(const char *prefix = nullptr);
}

#endif // AUTODOC_TESTS_UTILS_H
