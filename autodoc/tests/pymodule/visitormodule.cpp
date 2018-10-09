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
#include <memory>
#include <iostream>

#include "doxygen.h"
#include "config.h"
#include "configimpl.h"
#include "searchindex.h"
#include "store.h"
#include "qdir.h"

#include "docparser.h"
#include "filedef.h"
#include "autodoc/common/docutilstree.h"
#include "autodoc/common/visitor.h"

using namespace autodoc;

class Init
{
public:
    Init()
    {
        initDoxygen();
        Config::init();
        checkConfiguration();
        adjustConfiguration();
    }

    ~Init()
    {
        cleanUpDoxygen();
        finializeSearchIndexer();
        Config::deinit();
        delete Doxygen::symbolMap;
        delete Doxygen::clangUsrMap;
        delete Doxygen::symbolStorage;
    }
};


static PyObject* visitor_parse(PyObject *self, PyObject *args)
{
    const char *text;
    if (!PyArg_ParseTuple(args, "s", &text))
            return NULL;

    FileDef scope("/tests/", "testfile.h");
    MemberDef member(scope.absFilePath(), 10, 5, "void", "foo", "()", NULL,
                     Protection::Public, Specifier::Normal, false,
                     Relationship::Member, MemberType::MemberType_Function,
                     NULL, NULL);

    // convert the documentation string into an abstract syntax tree
    std::unique_ptr<DocNode> root(validatingParseDoc(
        "testfile.cpp",     // fileName,
        10,                  // lineNr,
        &scope,
        &member,
        text,
        FALSE,
        FALSE
    ));

    std::unique_ptr<DocutilsVisitor> visitor(new DocutilsVisitor(scope.absFilePath(), 10));
    root->accept(visitor.get());

    PyObject *res = visitor->takeDocument();
    if (!res)
        Py_RETURN_NONE;
    return res;
}


static PyMethodDef VisitorMethods[] = {
    {"parse",  visitor_parse, METH_VARARGS, "Parse doxygen docstring."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef visitormodule = {
   PyModuleDef_HEAD_INIT,
   "visitor",       /* name of module */
   "Test module",   /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   VisitorMethods
};


PyMODINIT_FUNC PyInit_visitor(void)
{
    static Init init;
    return PyModule_Create(&visitormodule);
}
