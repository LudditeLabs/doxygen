#include "autodoc/common/docblock.h"
#include "entry.h"

namespace autodoc {

void DocBlock::copyPositionFrom(Entry *entry)
{
    startLine = entry->docBlockLineStart;
    endLine = entry->docBlockLineEnd;
    startCol = entry->docBlockCol;
    endCol = entry->docBlockColEnd;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


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
