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

#include "autodoc/common/pyglobals.h"

namespace autodoc {

PyGlobals *PyGlobals::m_instance = nullptr;


PyGlobals::PyGlobals()
: m_valid(false)
{
    if (!setupDocutils() || !setupPickle())
        m_dumps.release();
    else
        m_valid = true;
}
//-----------------------------------------------------------------------------

PyGlobals::~PyGlobals()
{

}
//-----------------------------------------------------------------------------

PyGlobals *PyGlobals::instance()
{
    if (!m_instance)
        m_instance = new PyGlobals;
    return m_instance;
}
//-----------------------------------------------------------------------------

bool PyGlobals::setupDocutils()
{
    m_nodesMod = PyImport_ImportModule("docutils.nodes");
    if (!m_nodesMod)
        return printPyError("can't import docutils.nodes.");
    return true;
}
//-----------------------------------------------------------------------------

bool PyGlobals::setupPickle()
{
    // Cache pickle.dump

    PyObjectPtr pickle = PyImport_ImportModule("pickle");
    if (!pickle)
        return printPyError("can't import pickle.");

    m_dumps = PyObject_GetAttrString(pickle, "dumps");
    if (!m_dumps)
        return printPyError("can't get pickle.dumps().");

    // Cache io.open

    PyObjectPtr io = PyImport_ImportModule("io");
    if (!io)
        return printPyError("can't import io.");

    return true;
}
//-----------------------------------------------------------------------------

PyObject* PyGlobals::pickleToString(PyObject *object)
{
    if (!object || !isValid())
        return NULL;

    PyObject* res = PyObject_CallFunctionObjArgs(m_dumps, object, NULL);
    if (!res)
        printPyError("can't call pickle.dumps().");

    return res;
}
//-----------------------------------------------------------------------------

} // namespace autodoc
