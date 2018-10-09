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

    void generateDocBlocks(int refid, const Definition *ctx,
                           const MemberDef *member);

    int currentCompoundId() const { return m_currentCompoundId; }
    void setCurrentCompoundId(int id) { m_currentCompoundId = id; }

private:
    sqlite3 *m_db;
    InsertFileFunc m_insertFile;
    SqlStmt m_docblocksInsertStmt;
    int m_currentCompoundId;

    int save(int refid, int type, DocBlock *block, const char *bytes,
             size_t size);
};

} // namespace autodoc

#endif // AUTODOC_COMMON_CONTENTDB_H
