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

#ifndef AUTODOC_COMMON_DOCBLOCK_H
#define AUTODOC_COMMON_DOCBLOCK_H

#include <qcstring.h>
#include <qdict.h>

class Entry;

namespace autodoc {

struct DocBlock
{
    QCString filename;
    QCString doc;
    int      startLine;
    int      endLine;
    int      startCol;
    int      endCol;

    DocBlock(const QCString &filename, const QCString &doc, int startLine = 0,
             int endLine = 0, int startCol = 0, int endCol = 0)
        : filename(filename), doc(doc), startLine(startLine), endLine(endLine),
        startCol(startCol), endCol(endCol) {}

    DocBlock(const DocBlock &other)
        : filename(other.filename), doc(other.doc), startLine(other.startLine),
          endLine(other.endLine), startCol(other.startCol),
          endCol(other.endCol) {}
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


class DefinitionDoc
{
public:
    DefinitionDoc();
    DefinitionDoc(const DefinitionDoc &other);
    ~DefinitionDoc();

    bool isEmpty() const { return !m_blocks || m_blocks->isEmpty(); }
    const QDict<DocBlock>* blocks() const { return m_blocks; }

    DocBlock* get(const char *filename) const
    {
        return m_blocks ? m_blocks->find(filename) : NULL;
    }

    DocBlock* add(const QCString &filename, const QCString &doc,
                  int startLine = 0, int endLine = 0, int startCol = 0,
                  int endCol = 0);

private:
    QDict<DocBlock> *m_blocks;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace autodoc

#endif // AUTODOC_COMMON_DOCBLOCK_H
