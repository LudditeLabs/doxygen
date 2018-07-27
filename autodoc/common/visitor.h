#ifndef AUTODOC_COMMON_VISITOR_H
#define AUTODOC_COMMON_VISITOR_H

#include "Python.h"
#include <string>
#include <memory>
#include <qcstring.h>
#include "docvisitor.h"
#include "autodoc/common/utils.h"

#define NOTRACE_VISIT 1

#if defined(NOTRACE_VISIT)
# define TRACE_VISIT(text) ((void)0)
#else
# define TRACE_VISIT(text) printf(text)
#endif

class Definition;
class MemberDef;
class DocNode;

namespace autodoc {

class DocutilsTree;


PyObject* pickleDocTree(const QCString &fileName,
                        int lineNr,
                        Definition *scope,
                        MemberDef *md,
                        const QCString &text);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** This visitor builds python docutils document. */
class DocutilsVisitor: public DocVisitor
{
public:
    DocutilsVisitor(const QCString &fileName, int lineNumber);
    ~DocutilsVisitor();

    PyObject* document() const;
    PyObject* takeDocument();

    void visitPre(DocRoot *);
    void visitPost(DocRoot *);
    void visit(DocStyleChange *);
    void visitPre(DocPara *);
    void visitPost(DocPara *);
    void visitPre(DocText *);
    void visitPost(DocText *);
    void visitPre(DocParBlock *);
    void visitPost(DocParBlock *);

    // visit_inline.cpp
    void visit(DocWord *);
    void visit(DocWhiteSpace *);
    void visit(DocLinkedWord *);
    void visit(DocLineBreak *);
    void visit(DocSymbol *);
    void visit(DocURL *);
    void visit(DocHorRuler *);
    void visit(DocVerbatim *);
    void visit(DocAnchor *);
    void visit(DocInclude *);
    void visit(DocIncOperator *);
    void visit(DocFormula *);
    void visit(DocIndexEntry *);
    void visit(DocCite *);

    // visit_params.cpp
    void visitPre(DocParamSect *);
    void visitPost(DocParamSect *);
    void visitPre(DocParamList *);
    void visitPost(DocParamList *);

    // visit_simple_sect.cpp
    void visitPre(DocSimpleSect *);
    void visitPost(DocSimpleSect *);
    void visitPre(DocTitle *);
    void visitPost(DocTitle *);
    void visit(DocSimpleSectSep *);

    // visit_list.cpp
    void visitPre(DocAutoList *);
    void visitPost(DocAutoList *);
    void visitPre(DocAutoListItem *);
    void visitPost(DocAutoListItem *);
    void visitPre(DocSimpleList *);
    void visitPost(DocSimpleList *);
    void visitPre(DocSimpleListItem *);
    void visitPost(DocSimpleListItem *);
    void visitPre(DocHtmlList *);
    void visitPost(DocHtmlList *) ;
    void visitPre(DocHtmlListItem *);
    void visitPost(DocHtmlListItem *);
    void visitPre(DocHtmlDescList *);
    void visitPost(DocHtmlDescList *);
    void visitPre(DocHtmlDescTitle *);
    void visitPost(DocHtmlDescTitle *);
    void visitPre(DocHtmlDescData *);
    void visitPost(DocHtmlDescData *);

    // Not implemented yet.
    void visitPre(DocSection *);
    void visitPost(DocSection *);
    void visitPre(DocHtmlTable *);
    void visitPost(DocHtmlTable *);
    void visitPre(DocHtmlRow *);
    void visitPost(DocHtmlRow *);
    void visitPre(DocHtmlCell *);
    void visitPost(DocHtmlCell *);
    void visitPre(DocHtmlCaption *);
    void visitPost(DocHtmlCaption *);
    void visitPre(DocInternal *);
    void visitPost(DocInternal *);
    void visitPre(DocHRef *);
    void visitPost(DocHRef *);
    void visitPre(DocHtmlHeader *);
    void visitPost(DocHtmlHeader *);
    void visitPre(DocImage *);
    void visitPost(DocImage *);
    void visitPre(DocDotFile *);
    void visitPost(DocDotFile *);
    void visitPre(DocMscFile *);
    void visitPost(DocMscFile *);
    void visitPre(DocDiaFile *);
    void visitPost(DocDiaFile *);
    void visitPre(DocLink *);
    void visitPost(DocLink *);
    void visitPre(DocRef *);
    void visitPost(DocRef *);
    void visitPre(DocSecRefItem *);
    void visitPost(DocSecRefItem *);
    void visitPre(DocSecRefList *);
    void visitPost(DocSecRefList *);
    void visitPre(DocXRefItem *);
    void visitPost(DocXRefItem *);
    void visitPre(DocInternalRef *);
    void visitPost(DocInternalRef *);
    void visitPre(DocCopy *);
    void visitPost(DocCopy *);
    void visitPre(DocHtmlBlockQuote *);
    void visitPost(DocHtmlBlockQuote *);
    void visitPre(DocVhdlFlow *);
    void visitPost(DocVhdlFlow *);

private:
    std::unique_ptr<DocutilsTree> m_tree;
    PyObjectPtr m_fieldList;

    QCString m_fileName;
    int m_lineNumber;
    QCString m_textBuf;

    int m_paramType;
    bool m_skipNextWhitespace;  // Don't add next whitespace.

    // Skip node processing if not -1.
    // Value describes which node (its kind) sets the restriction.
    int m_skipProcessing;

    int m_titleParent;

    /**
     * Create text node from content in m_textBuf.
     */
    bool maybeCreateTextNode(bool stripTrailing = false);
    bool maybeFinishCurrentPara(DocNode *node);

    bool beforePre(DocNode *node);
    bool beforePost(DocNode *node);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace autodoc

#endif // AUTODOC_COMMON_VISITOR_H
