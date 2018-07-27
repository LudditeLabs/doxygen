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

    void copyPositionFrom(Entry *entry);
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
