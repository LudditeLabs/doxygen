#include "Python.h"
#include "autodoc/common/contentdb.h"
#include "autodoc/common/docblock.h"
#include "autodoc/common/docutilstree.h"
#include "autodoc/common/visitor.h"
#include "message.h"
#include "definition.h"
#include "memberdef.h"
#include <sqlite3.h>

static const char* schema[][2] = {
    { "docblocks",
      "CREATE TABLE IF NOT EXISTS docblocks (\n\t"
      "rowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n\t"
      "id_member    INTEGER NOT NULL,\n\t"
      "kind         INTEGER NOT NULL,  -- 0:member 1:compound\n\t"
      "id_file      INTEGER NOT NULL,\n\t"
      "start_line   INTEGER,\n\t"
      "start_col    INTEGER,\n\t"
      "end_line     INTEGER,\n\t"
      "end_col      INTEGER,\n\t"
      "docstring    TEXT,\n\t"
      "doc          BLOB\n"
      ");\n"
      "CREATE UNIQUE INDEX idx_docblocks ON docblocks\n"
      "\t(id_member);"
    }
};


namespace autodoc
{

ContentDb::ContentDb(sqlite3 *db, InsertFileFunc insertFileFunc)
: m_db(db), m_insertFile(insertFileFunc), m_currentCompoundId(-1)
{
    m_docblocksInsertStmt.query =
        "INSERT INTO docblocks "
        "(id_member,kind,id_file,start_line,start_col,end_line,end_col,docstring,doc) "
        "VALUES "
        "(:id_member,:kind,:id_file,:start_line,:start_col,:end_line,:end_col,:docstring,:doc)";
    m_docblocksInsertStmt.db = NULL;
    m_docblocksInsertStmt.stmt = NULL;
}
//-----------------------------------------------------------------------------

ContentDb::~ContentDb()
{

}
//-----------------------------------------------------------------------------

// copy-paste from sqlite3gen.cpp
bool ContentDb::initializeSchema()
{
    int rc;

    for (unsigned int k = 0; k < sizeof(schema) / sizeof(schema[0]); k++)
    {
        const char *q = schema[k][1];
        char *errmsg;
        rc = sqlite3_exec(m_db, q, NULL, NULL, &errmsg);
        if (rc != SQLITE_OK)
        {
            msg("failed to execute query: %s\n\t%s\n", q, errmsg);
            return false;
        }
    }

    return true;
}
//-----------------------------------------------------------------------------

bool ContentDb::prepareStatements()
{
    return prepareStatement(m_db, m_docblocksInsertStmt) != -1;
}
//-----------------------------------------------------------------------------

void ContentDb::generateDocBlocks(int memberId, int kind, const Definition *ctx,
                                  const MemberDef *member)
{
    const Definition *def = member ? member : ctx;

    DefinitionDoc *docs = def->docs();
    if (!docs)
        return;

    const QDict<DocBlock> *blocks = docs->blocks();
    if (!blocks)
        return;

    msg("=== MEMBER DOC: %s\n", def->name().data());
    QDictIterator<DocBlock> it(*blocks);
    DocBlock *block;
    for (it.toFirst(); (block = it.current()); ++it)
    {
        msg("===== %s\n", block->filename.data());
        msg("===== (%d; %d), (%d; %d)\n", block->startLine, block->startCol,
            block->endLine, block->endCol);

        // Postpone docstrings parsing for python files.
        // This will be done on python side.
        if (block->filename.right(3) == ".py")
        {
            msg("===== Skip pickle.\n");
            save(memberId, kind, block, nullptr, 0);
            continue;
        }

        // Python: pickled docutils document.
        // TODO: is const cast valid here?
        PyObjectPtr res = pickleDocTree(
            block->filename,
            def->getDefLine(),
            const_cast<Definition*>(ctx),
            const_cast<MemberDef*>(member),
            block->doc);

        if (res)
        {
            char *bytes;
            Py_ssize_t size;

            if (PyBytes_AsStringAndSize(res, &bytes, &size) == -1)
            {
                printPyError("can't pickle object.");
                continue;
            }
            save(memberId, kind, block, bytes, size);
        }
    }
}
//-----------------------------------------------------------------------------

int ContentDb::save(int memberId, int kind, DocBlock *block, const char *bytes,
                    size_t size)
{
    int id_file = (*m_insertFile)(block->filename);

    bindIntParameter(m_docblocksInsertStmt,":id_member", memberId);
    bindIntParameter(m_docblocksInsertStmt,":kind", kind);
    bindIntParameter(m_docblocksInsertStmt,":id_file", id_file);
    bindIntParameter(m_docblocksInsertStmt,":start_line", block->startLine);
    bindIntParameter(m_docblocksInsertStmt,":start_col", block->startCol);
    bindIntParameter(m_docblocksInsertStmt,":end_line", block->endLine);
    bindIntParameter(m_docblocksInsertStmt,":end_col", block->endCol);
    bindTextParameter(m_docblocksInsertStmt,":docstring",
                      block->doc.isEmpty() ? "" : block->doc.data());
    if (bytes != nullptr)
        bindBlobParameter(m_docblocksInsertStmt,":doc", bytes, size);
    return step(m_docblocksInsertStmt,TRUE);
}

} // namespace autodoc
