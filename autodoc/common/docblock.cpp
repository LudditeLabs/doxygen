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

#include "autodoc/common/docblock.h"
#include "entry.h"

namespace autodoc {


DefinitionDoc::DefinitionDoc()
: m_blocks(0)
{

}
//-----------------------------------------------------------------------------

DefinitionDoc::DefinitionDoc(const DefinitionDoc &other)
: m_blocks(0)
{
    if (other.m_blocks)
    {
        m_blocks = new QDict<DocBlock>;
        m_blocks->setAutoDelete(true);

        QDictIterator<DocBlock> it(*other.m_blocks);
        DocBlock *block;
        for (it.toFirst(); (block = it.current()); ++it)
            m_blocks->insert(block->filename, new DocBlock(*block));
    }
}
//-----------------------------------------------------------------------------

DefinitionDoc::~DefinitionDoc()
{
    delete m_blocks;
}
//-----------------------------------------------------------------------------

DocBlock* DefinitionDoc::add(const QCString &filename, const QCString &doc,
                             int startLine, int endLine, int startCol, int endCol)
{
    if (!m_blocks)
    {
        m_blocks = new QDict<DocBlock>;
        m_blocks->setAutoDelete(true);
    }

    DocBlock *block = new DocBlock(filename, doc, startLine, endLine,
                                   startCol, endCol);
    m_blocks->insert(filename, block);
    return block;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace autodoc
