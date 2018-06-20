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
#include "autodoc/pynode.h"
#include "autodoc/visitor/visitor.h"


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

    std::unique_ptr<PyDocVisitor> visitor(new PyDocVisitor(scope.absFilePath(), 10));
    root->accept(visitor.get());

    return visitor->takeDocument();
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
