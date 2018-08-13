#ifndef AUTODOC_COMMON_CONTENTDB_H
#define AUTODOC_COMMON_CONTENTDB_H

#include "sqlite3utils.h"

class Definition;
class MemberDef;

typedef int (*InsertFileFunc)(const char*);

namespace autodoc {

struct DocBlock;

class ContentDb
{
public:
    ContentDb(sqlite3 *db, InsertFileFunc insertFileFunc);
    ~ContentDb();

    bool initializeSchema();
    bool prepareStatements();

    void generateDocBlocks(int memberId, int kind, const Definition *ctx,
                           const MemberDef *member);

private:
    sqlite3 *m_db;
    InsertFileFunc m_insertFile;
    SqlStmt m_docblocksInsertStmt;

    int save(int id, int kind, DocBlock *block, const char *bytes, size_t size);
};

} // namespace autodoc

#endif // AUTODOC_COMMON_CONTENTDB_H
