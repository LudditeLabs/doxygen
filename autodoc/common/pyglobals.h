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

#ifndef AUTODOC_COMMON_PYGLOBALS_H
#define AUTODOC_COMMON_PYGLOBALS_H

#include "Python.h"
#include <qstringlist.h>
#include <deque>
#include "autodoc/common/utils.h"

namespace autodoc {

/**
 * This class provides access to some python functions and modules.
 */
class PyGlobals
{
public:
    ~PyGlobals();

    static PyGlobals* instance();

    bool isValid() const { return m_valid; }

    /**
     * Get node class from <tt>docutils.nodes</tt>.
     *
     * @param Node class name.
     * @return Class object.
     */
    PyObject* nodeClass(const QCString &name) const
    {
        return PyObject_GetAttrString(m_nodesMod, name.data());
    }

    PyObject* pickleToString(PyObject *object);

private:
    static PyGlobals *m_instance;
    PyObjectPtr m_nodesMod;
    PyObjectPtr m_dumps;
    bool m_valid;

    PyGlobals();

    bool setupDocutils();
    bool setupPickle();
};

} // namespace autodoc

#endif // AUTODOC_COMMON_PYGLOBALS_H
